#include "sudoku.h"
#include <iostream>
#include <unordered_set>
#include <time.h>
#include <Windows.h>


#define DANCE_LINK_COLS 81*4

const std::unordered_set<int> ALLOWEDSET = {-1,1,2,3,4,5,6,7,8,9};

void LogError(const char* strerrmsg)
{
    std::cout << strerrmsg << std::endl;
}

bool CheckInput(const sudoku_vec& inputs)
{
    auto m = inputs.size();
    auto n = (m > 0) ? inputs[0].size() : 0;
    if(m != 9 || n != 9)
    {
        LogError("matrix is not square");
        return false;
    }
    for(auto row : inputs)
    {
        for(auto col: row)
        {
            if(ALLOWEDSET.find(col) == ALLOWEDSET.end())
            {
                LogError("col not in [-1,1,2,3,4,5,6,7,8,9]");
                return false;
            }                
        } 
    }
    return true;
}

//获取当前位置可用数字
std::unordered_set<int> Getalternative(sudoku_vec& inputs, int m, int row, int col)
{
    std::unordered_set<int> res = {1,2,3,4,5,6,7,8,9};
    int square_row = row / 3, square_col = col/3;
    for(auto line = 0; line < m; ++line)
    {
        if(row != line && inputs[line][col] != -1)
        {
            auto iter = res.find(inputs[line][col]);
            if(iter != res.end())
                res.erase(iter);
        }
        if(col != line && inputs[row][line] != -1)
        {
            auto iter = res.find(inputs[row][line]);
            if(iter != res.end())
                res.erase(iter);
        }
    }
    for(auto line1 = 0; line1 < 3; ++line1)
    {
        for(auto line2 = 0; line2 < 3; ++line2)
        {
            if(row != square_row*3+line1 &&  col != square_col*3+line2 && inputs[square_row*3+line1][square_col*3+line2] != -1)
            {
                auto iter = res.find(inputs[square_row*3+line1][square_col*3+line2]);
                if(iter != res.end())
                    res.erase(iter);
            }
        }
    }
    return res;
}

bool DFS(sudoku_vec& inputs, int m, int rb, int cb)
{
    while(inputs[rb][cb] != -1)
    {
        ++cb;
        if(cb >= m)
        {
            cb = 0;
            ++rb;
        }
        if(rb >= m)
            return true;
    }
    auto alts = Getalternative(inputs, m, rb, cb);
    for(auto alt : alts)
    {
        inputs[rb][cb] = alt;
        if(DFS(inputs, m, rb, cb))
            return true;
        inputs[rb][cb] = -1;
    }
    return false;
}

bool FindSudokuAnswer(sudoku_vec& inputs)
{
    if(!CheckInput(inputs))
        return false;
    auto m = inputs.size();
    return DFS(inputs, m, 0, 0);
}


struct Dance_Link_Node
{
    bool bAcc = false;
    int count = 1;
    int value = -1;
    int col = -1;
    int row = -1;
    int ori_row = -1;
    int ori_col = -1;
    Dance_Link_Node* pre = nullptr;
    Dance_Link_Node* next = nullptr;
    Dance_Link_Node* up = nullptr;
    Dance_Link_Node* down = nullptr;
};


struct Dance_Link
{
    Dance_Link_Node* head;
    Dance_Link_Node* cols[DANCE_LINK_COLS];
    Dance_Link(){
        head = new Dance_Link_Node();
        for(int i = 0; i < DANCE_LINK_COLS; ++i)
        {
            cols[i] = new Dance_Link_Node();
            if(i != 0)
            {
                cols[i]->pre = cols[i-1];
                cols[i-1]->next = cols[i];
            }
            else
            {
                cols[i]->pre = head;
                head->next = cols[i];
            }
            cols[i]->up = cols[i];
            cols[i]->down = cols[i];
        }
        cols[DANCE_LINK_COLS-1]->next = head;
        head->pre = cols[DANCE_LINK_COLS-1];
    }

    ~Dance_Link()
    {
        Dance_Link_Node* pHnode = head->next;
        Dance_Link_Node* pHnodeDel = pHnode->next;
        while(pHnode != head)
        {
            delete pHnode;
            pHnode = NULL;
            pHnode = pHnodeDel;
            pHnodeDel = pHnodeDel->next;
        }
        delete head;
        head = NULL;
    }

};


void InitDanceLink(const sudoku_vec& inputs, int m, Dance_Link* link)
{ 
    int rowCount = 0;
    for(int row=0; row<m; ++row)
    {
        for(int col=0; col < m; ++col)
        {
            if(inputs[row][col] == -1)
            {
                for(int k = 1; k < m+1; ++k)
                {
                    int g = 3*(row / 3) + col/3;
                    int tcols[4] = {row*9+col, 81+row*9+k-1, 162+col*9+k-1,243+g*9+k-1};
                    for(int i = 0; i < 4; ++i)
                    {
                        Dance_Link_Node* pNode = new Dance_Link_Node();
                        pNode->down = link->cols[tcols[i]];
                        pNode->up = link->cols[tcols[i]]->up;
                        link->cols[tcols[i]]->up->down = pNode;
                        link->cols[tcols[i]]->up = pNode;
                        pNode->value = 1;
                        pNode->col = tcols[i];
                        pNode->row = rowCount;
                        pNode->ori_row = row;
                        pNode->ori_col = col;
                        if(i != 0)
                        {
                            link->cols[tcols[i]]->up->pre = link->cols[tcols[i-1]]->up;
                            link->cols[tcols[i-1]]->up->next = link->cols[tcols[i]]->up;
                        }
                    }
                    link->cols[tcols[0]]->up->pre = link->cols[tcols[3]]->up;
                    link->cols[tcols[3]]->up->next = link->cols[tcols[0]]->up;
                    rowCount++;
                }
            }
            else
            {
                int g = 3*(row / 3) + col/3;
                int tcols[4] = {row*9+col, 81+row*9+inputs[row][col]-1, 162+col*9+inputs[row][col]-1,243+g*9+inputs[row][col]-1};
                for(int i = 0; i < 4; ++i)
                {
                    Dance_Link_Node* pNode = new Dance_Link_Node();
                    pNode->down = link->cols[tcols[i]];
                    pNode->up = link->cols[tcols[i]]->up;
                    link->cols[tcols[i]]->up->down = pNode;
                    link->cols[tcols[i]]->up = pNode;
                    pNode->bAcc = true;
                    pNode->value = 1;
                    pNode->col = tcols[i];
                    pNode->row = rowCount;
                    pNode->ori_row = row;
                    pNode->ori_col = col;
                    if(i != 0)
                    {
                        link->cols[tcols[i]]->up->pre = link->cols[tcols[i-1]]->up;
                        link->cols[tcols[i-1]]->up->next = link->cols[tcols[i]]->up;
                    }
                }
                link->cols[tcols[0]]->up->pre = link->cols[tcols[3]]->up;
                link->cols[tcols[3]]->up->next = link->cols[tcols[0]]->up;
                rowCount++;
            }
        }
    }
}


void InitDanceLink2(const sudoku_vec& inputs, int m, Dance_Link* link)
{ 
    int rowCount = 0;
    for(int row=0; row<m; ++row)
    {
        for(int col=0; col < m; ++col)
        {
            if(inputs[row][col] != -1)
            {
                int g = 3*(row / 3) + col/3;
                int tcols[4] = {row*9+col, 81+row*9+inputs[row][col]-1, 162+col*9+inputs[row][col]-1,243+g*9+inputs[row][col]-1};
                for(int i = 0; i < 4; ++i)
                {
                    link->cols[tcols[i]]->count = -1;
                    Dance_Link_Node* pNode = new Dance_Link_Node();
                    pNode->down = link->cols[tcols[i]];
                    pNode->up = link->cols[tcols[i]]->up;
                    link->cols[tcols[i]]->up->down = pNode;
                    link->cols[tcols[i]]->up = pNode;
                    pNode->bAcc = true;
                    pNode->value = 1;
                    pNode->col = tcols[i];
                    pNode->row = rowCount;
                    pNode->ori_row = row;
                    pNode->ori_col = col;
                    if(i != 0)
                    {
                        link->cols[tcols[i]]->up->pre = link->cols[tcols[i-1]]->up;
                        link->cols[tcols[i-1]]->up->next = link->cols[tcols[i]]->up;
                    }
                    link->cols[tcols[i]]->next->pre = link->cols[tcols[i]]->pre;
                    link->cols[tcols[i]]->pre->next = link->cols[tcols[i]]->next;
                }
                link->cols[tcols[0]]->up->pre = link->cols[tcols[3]]->up;
                link->cols[tcols[3]]->up->next = link->cols[tcols[0]]->up;
                rowCount++;
            }
        }
    }
    for(int row=0; row<m; ++row)
    {
        for(int col=0; col < m; ++col)
        {
            if(inputs[row][col] == -1)
            {
                for(int k = 1; k < m+1; ++k)
                {
                    int g = 3*(row / 3) + col/3;
                    int tcols[4] = {row*9+col, 81+row*9+k-1, 162+col*9+k-1,243+g*9+k-1};
                    if(link->cols[tcols[0]]->count == -1 || link->cols[tcols[1]]->count == -1 ||
                        link->cols[tcols[2]]->count == -1 || link->cols[tcols[3]]->count == -1)
                        continue;
                    for(int i = 0; i < 4; ++i)
                    {
                        Dance_Link_Node* pNode = new Dance_Link_Node();
                        pNode->down = link->cols[tcols[i]];
                        pNode->up = link->cols[tcols[i]]->up;
                        link->cols[tcols[i]]->up->down = pNode;
                        link->cols[tcols[i]]->up = pNode;
                        pNode->value = 1;
                        pNode->col = tcols[i];
                        pNode->row = rowCount;
                        pNode->ori_row = row;
                        pNode->ori_col = col;
                        if(i != 0)
                        {
                            link->cols[tcols[i]]->up->pre = link->cols[tcols[i-1]]->up;
                            link->cols[tcols[i-1]]->up->next = link->cols[tcols[i]]->up;
                        }
                    }
                    link->cols[tcols[0]]->up->pre = link->cols[tcols[3]]->up;
                    link->cols[tcols[3]]->up->next = link->cols[tcols[0]]->up;
                    rowCount++;
                }
            }
        }
    }
    
}


bool FindAccurateCoverage(sudoku_vec& inputs, int m, Dance_Link* link)
{
    if(link->head->next == link->head || link->head->pre == link->head)
        return true;
    Dance_Link_Node* pNode = link->head->next;
    if(link->head->next != link->head && (pNode->up == pNode || pNode->down == pNode))
        return false;
    Dance_Link_Node* nNode = pNode->down;
    link->head->next = pNode->next;
    pNode->next->pre = link->head;
    Dance_Link_Node* rNode;
    Dance_Link_Node* cNode;
    Dance_Link_Node* ctNode;
    int count;
    while(nNode != pNode)
    {
        if(nNode == link->cols[nNode->col])
            continue;
        Dance_Link_Node* lNode = nNode;
        count = 0;
        do
        {
            if(count ==3)
            {
                int g = 3*(lNode->ori_row / 3) + lNode->ori_col/3;
                if(!pNode->bAcc)
                    inputs[lNode->ori_row][lNode->ori_col] = lNode->col-243-g*9+1;
            }
            count += 1;
            lNode = lNode->next;
        }while(lNode != nNode);
        //删除节点
        rNode = nNode;
        do
        {
            cNode = rNode->down;
            while (cNode != rNode)
            {
                if(cNode == link->cols[rNode->col])
                {
                    cNode = cNode->down;
                    continue;
                }
                ctNode = cNode->next;
                while(cNode != ctNode)
                {
                    ctNode->up->down = ctNode->down;
                    ctNode->down->up = ctNode->up;
                    ctNode = ctNode->next;
                }
                cNode = cNode->down;
            } 
            link->cols[rNode->col]->pre->next = link->cols[rNode->col]->next;
            link->cols[rNode->col]->next->pre = link->cols[rNode->col]->pre;
            rNode = rNode->next;
        }while(rNode != nNode);
        if(FindAccurateCoverage(inputs, m, link))
            return true;
        //恢复节点
        rNode = nNode;
        do
        {
            cNode = rNode->down;
            while(cNode != rNode)
            {
                if(cNode == link->cols[rNode->col])
                {
                    cNode = cNode->down;
                    continue;
                }
                ctNode = cNode->next;
                while(cNode != ctNode)
                {
                    ctNode->up->down = ctNode;
                    ctNode->down->up = ctNode;
                    ctNode = ctNode->next;
                }
                cNode = cNode->down;
            }
            link->cols[rNode->col]->pre->next = link->cols[rNode->col];
            link->cols[rNode->col]->next->pre = link->cols[rNode->col];
            rNode = rNode->next;
        }while(rNode != nNode);
        lNode = nNode;
        count = 0;
        do
        {
            if(count ==3 && !lNode->bAcc)
                inputs[lNode->ori_row][lNode->ori_col] = -1;
            count += 1;
            lNode = lNode->next;
        }while(lNode != nNode);
        nNode = nNode->down;
    }
    link->head->next = pNode;
    pNode->next->pre = pNode;
    return false;
}


//原始的舞蹈链算法
bool FindSudokuAnswer2(sudoku_vec& inputs)
{
    if(!CheckInput(inputs))
        return false;
    Dance_Link link;
    auto m = inputs.size();
    InitDanceLink(inputs, m, &link);
    return FindAccurateCoverage(inputs, m, &link);
}


//修改过的舞蹈链算法
bool FindSudokuAnswer3(sudoku_vec& inputs)
{
    if(!CheckInput(inputs))
        return false;
    Dance_Link link;
    auto m = inputs.size();
    InitDanceLink2(inputs, m, &link);
    return FindAccurateCoverage(inputs, m, &link);
}
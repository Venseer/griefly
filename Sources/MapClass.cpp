#include <math.h>
#include <sstream>
#include <iostream>
#include <assert.h>
#include <hash_map>

#include "MapClass.h"
#include "MainInt.h"
#include "TileInt.h"

#include "mob_position.h"

bool MapMaster::CheckDublicate()
{
    assert(false && "Not used");
    return true;
}

void MapMaster::Draw()
{
    if(!canDraw())
        return;
    if(!mobi->visiblePoint) 
        return;
    glClear(GL_COLOR_BUFFER_BIT);
    int z_level_m = castTo<CubeTile>(mobi->thisMob->GetOwner().ret_item())->posz();
    for (int z_level = 0; z_level < sizeDmap; z_level++) 
    {
        for(int i = 0; i < MAX_LEVEL; ++i)
        {
            auto it2 = mobi->visiblePoint->begin();
            while(it2 != mobi->visiblePoint->end())
            {   
                if(checkOutBorder(it2->posx, it2->posy))
                    if (it2->posz == z_level)
                        squares[it2->posx][it2->posy][it2->posz]->ForEach([&](id_ptr_on<IOnMapBase> item)
                        {
                            auto item_n = castTo<IOnMapItem>(item.ret_item());
                            if (item_n->v_level == i)
                                item_n->processImage(z_level < z_level_m ? TOP : SAME);//screen
                        });
                ++it2;
            }
        } 
        auto it2 = mobi->visiblePoint->begin();
        while(it2 != mobi->visiblePoint->end())
        {   
            if(checkOutBorder(it2->posx, it2->posy))
                if (it2->posz == z_level)
                    squares[it2->posx][it2->posy][it2->posz]->ForEach([&](id_ptr_on<IOnMapBase> item)
                    {
                        auto item_n = castTo<IOnMapItem>(item.ret_item());
                        if (item_n->v_level >= MAX_LEVEL)
                            item_n->processImage(z_level < z_level_m ? TOP : SAME);//screen
                    });
            ++it2;
        }
    }
};

void MapMaster::makeTiles()
{
    // Gen tiles
    for(int x = 0; x < sizeWmap; x++)
    {
        for(int y = 0; y < sizeHmap; y++) 
        {
            for (int z = 0; z < sizeDmap; z++)
            {
                auto loc = IMainItem::fabric->newItem<CubeTile>(0, CubeTile::T_ITEM_S());
                loc->SetPos(x, y, z);
                squares[x][y][z] = loc;
            }
        }
    }
}

void MapMaster::makeMap()
{
    // End gen
    for(int x = 0; x < sizeWmap; x++)
    {
        for(int y = 0; y < sizeHmap; y++) 
        {     
            // Ge
            //
            bool chk = (rand() % 10 != 1);
            id_ptr_on<IOnMapItem> loc = IMainItem::fabric->newItemOnMap<IOnMapItem>(hash(chk ? "ground" : "pit"), squares[x][y][0]);
            loc->imageStateH = 0;
            if (chk)
                loc->imageStateW = rand() % 4;
            else
                loc->imageStateW = 0;
            
            if(rand() % 29 == 1 || x == 0 || y == 0 || x == sizeWmap - 1 || y == sizeHmap - 1)
                IMainItem::fabric->newItemOnMap<IOnMapItem>(hash("testmob"), squares[x][y][1]);
            if(rand() % 60 == 1 && x != 0 && y != 0 && x != sizeWmap - 1 && y != sizeHmap - 1)
                IMainItem::fabric->newItemOnMap<IOnMapItem>(hash("kivsjak"), squares[x][y][1]);
            if(rand() % 3 == 1 && x != 0 && y != 0 && x != sizeWmap - 1 && y != sizeHmap - 1)
                IMainItem::fabric->newItemOnMap<IOnMapItem>(hash("weed"), squares[x][y][1]);//*/
        }
    }
    SYSTEM_STREAM << "End create map\n";
};

void MapMaster::centerFromTo(int nowPosx, int nowPosy, int nowPosz)
{
    // ?
};

MapMaster::MapMaster()
{
    ms_last_draw = 0;
    pathf.map = this;
    losf.map = this;  
};

bool MapMaster::canDraw()
{
    if (SDL_GetTicks() - ms_last_draw > static_cast<size_t>(1000 / DRAW_MAX + 1))
    {
        ms_last_draw = SDL_GetTicks();
        return true;
    }
    return false;
}

void CPathFinder::clearPathfinding()
{
    for(int i = 0; i < sizeHmap; ++i)
    {
        for(int j = 0; j < sizeWmap; ++j)
        {
            squares[i][j].before = 0;
            squares[i][j].inCloseList = false;
            squares[i][j].inOpenList = false;
            squares[i][j].cost = 0;
            squares[i][j].posx = i;
            squares[i][j].posy = j;
        }
    }
    openList.clear();
};


bool MapMaster::isPassable(int posx, int posy, int posz)
{
    return squares[posx][posy][posz]->IsPassable();
};

void MapMaster::switchDir(int& posx, int& posy, Dir direct, int num, bool back)//TODO: Remove back
{
    if(!back)
    {
        switch(direct)
        {
        case D_UP:
            posy -= num;
            return;
        case D_DOWN:
            posy += num;
            return;
        case D_LEFT:
            posx -= num;
            return;
        case D_RIGHT:
            posx += num;
            return;
        }
    }
    else
    {
        switch(direct)
        {
        case D_UP:
            posy += num;
            return;
        case D_DOWN:
            posy -= num;
            return;
        case D_LEFT:
            posx += num;
            return;
        case D_RIGHT:
            posx -= num;
            return;
        }
    }
};

bool MapMaster::checkOutBorder(int posx, int posy)
{
    if(posy < 0 || posy > (sizeHmap - 1) || posx < 0 || posx > sizeWmap - 1) return false;
    return true;
};

bool MapMaster::checkOutBorder(int posx, int posy, Dir direct)
{
    if((direct == D_UP && posy <= 0) || (direct == D_DOWN && posy >= (sizeHmap - 1)) || (direct == D_LEFT && posx <= 0) || (direct == D_RIGHT && posx >= (sizeWmap - 1))) return false;
    return true;
};

bool MapMaster::isVisible(int posx, int posy, int posz)
{
    if(!checkOutBorder(posx, posy/*TODO: posz*/))
        return false;
    return squares[posx][posy][posz]->IsTransparent();
};

void MapMaster::splashLiquid(std::list<HashAmount> ha, int posx, int posy, int posz)
{
    // TODO:
    /*std::list<HashAmount>* loc = &ha;
    auto it = squares[posx][posy][].end();
    if(it == squares[posx][posy].begin())
        return;
    do
    {
        --it;
        std::list<HashAmount> ha = (*it)->insertLiquid(*loc);
    }
    while(ha.size() && it != squares[posx][posy].begin());*/
}

id_ptr_on<IOnMapItem> MapMaster::click(int x, int y)
{
    if(!mobi->visiblePoint) 
        return 0;

    // Due to resize emulate some shit
    SYSTEM_STREAM << "NOW CLICK X: " << x << ", ";
    x = static_cast<int>(static_cast<float>(x) * (static_cast<float>(sizeW) / static_cast<float>(screen->w())));
    SYSTEM_STREAM << x << std::endl;
    SYSTEM_STREAM << "NOW CLICK Y: " << y << ", ";
    y = static_cast<int>(static_cast<float>(y) * (static_cast<float>(sizeH) / static_cast<float>(screen->h())));
    SYSTEM_STREAM << y << std::endl;
    //

    id_ptr_on<IOnMapItem> retval = 0;

    int z_level_m = castTo<CubeTile>(mobi->thisMob->GetOwner().ret_item())->posz();
    for (int z = z_level_m; z >= 0; --z)
    {
        auto it2 = mobi->visiblePoint->begin();  
        while(it2 != mobi->visiblePoint->end())
        {
            if (it2->posz == z)
                squares[it2->posx][it2->posy][it2->posz]->ForEach([&](id_ptr_on<IOnMapBase> item_h)
                {
                    auto item = castTo<IOnMapItem>(item_h.ret_item());
                    if (retval.ret_id() == 0)
                        if(item->v_level >= MAX_LEVEL)
                            if(!item->IsTransp(
                                x - (item->GetDrawX() + mob_position::get_shift_x()),
                                y - (item->GetDrawY() + mob_position::get_shift_y())))
                                retval = item_h;
                });
            if (retval.valid())
                return retval;
            ++it2;
        }
        it2 = mobi->visiblePoint->begin();  
        for(int i = MAX_LEVEL - 1; i >= 0; --i)
        {
            auto it2 = mobi->visiblePoint->begin();  
            while(it2 != mobi->visiblePoint->end())
            {
                if (it2->posz == z)
                    squares[it2->posx][it2->posy][it2->posz]->ForEach([&](id_ptr_on<IOnMapBase> item_h)
                    {
                        auto item = castTo<IOnMapItem>(item_h.ret_item());
                        if (retval.ret_id() == 0)
                            if(item->v_level == i)
                                if(!item->IsTransp(
                                    x - (item->GetDrawX() + mob_position::get_shift_x()),
                                    y - (item->GetDrawY() + mob_position::get_shift_y())))
                                    retval = item_h;
                    });
                if (retval.ret_id())
                    return retval;
                ++it2;
            }
        }
    }
    return 0;
}

std::list<Dir> CPathFinder::calculatePath(int fromPosx, int fromPosy, int toPosx, int toPosy, int toPosz)
{
    int begTime = SDL_GetTicks();
    ++numOfPathfind;
    clearPathfinding();
    std::list<Dir> path;
    int locposx = fromPosx;
    int locposy = fromPosy;
    squares[locposx][locposy].realcost = 0;
    squares[locposx][locposy].cost = calcCost(locposx, locposy, toPosx, toPosy);
    squares[locposx][locposy].inOpenList = true;
    addToOpen(locposx, locposy);
    int i = 0;
    while(locposx != toPosx || locposy != toPosy)
    {
        i++;

        addNear(locposx, locposy, toPosx, toPosy);
        squares[locposx][locposy].inOpenList = false;
        squares[locposx][locposy].inCloseList = true;
        openList.pop_front();
        
        if(openList.begin() == openList.end()) 
            return path;
        locposx = (*openList.begin())->posx;
        locposy = (*openList.begin())->posy;
    }
    //printf("\n%d - number seen squares\n", i);
    square* sq = &squares[toPosx][toPosy];
    while(sq != &squares[fromPosx][fromPosy])
    {
        if(sq->posx < sq->before->posx) 
            path.push_front(D_LEFT);
        else if(sq->posx > sq->before->posx) 
            path.push_front(D_RIGHT);
        else if(sq->posy < sq->before->posy) 
            path.push_front(D_UP);
        else 
            path.push_front(D_DOWN);
        sq = sq->before;
    }
    //SYSTEM_STREAM << SDL_GetTicks() - begTime << " Pathspeed\n";
    return path;
};


void CPathFinder::addNear(int posx, int posy, int toPosx, int toPosy)
{
    //printf("%d %d huj\n", posx, posy);

    if(!(squares[posx + 1][posy].inCloseList) && map->isPassable(posx + 1, posy))
    {
        if(squares[posx + 1][posy].inOpenList)
        {
            if(squares[posx][posy].realcost + 1 < squares[posx + 1][posy].realcost)
            {
                squares[posx + 1][posy].realcost = squares[posx][posy].realcost + 1;
                squares[posx + 1][posy].cost = calcCost(posx, posy, toPosx, toPosy);
                squares[posx + 1][posy].before = &squares[posx][posy];
                removeFromOpen(posx + 1, posy);
                addToOpen(posx + 1, posy);
            }
        }
        else
        {
            squares[posx + 1][posy].realcost = squares[posx][posy].realcost + 1;
            squares[posx + 1][posy].cost = calcCost(posx, posy, toPosx, toPosy);
            squares[posx + 1][posy].before = &squares[posx][posy];
            squares[posx + 1][posy].inOpenList = true;
            addToOpen(posx + 1, posy);
        }
    }
    //printf("Wtf!\n");
    if(!squares[posx - 1][posy].inCloseList && map->isPassable(posx - 1, posy))
    {
        if(squares[posx - 1][posy].inOpenList)
        {
            if(squares[posx][posy].realcost + 1 < squares[posx - 1][posy].realcost)
            {
                squares[posx - 1][posy].realcost = squares[posx][posy].realcost + 1;
                squares[posx - 1][posy].cost = calcCost(posx, posy, toPosx, toPosy);
                squares[posx - 1][posy].before = &squares[posx][posy];
                removeFromOpen(posx - 1, posy);
                addToOpen(posx - 1, posy);
            }
        }
        else
        {
            squares[posx - 1][posy].realcost = squares[posx][posy].realcost + 1;
            squares[posx - 1][posy].cost = calcCost(posx, posy, toPosx, toPosy);
            squares[posx - 1][posy].before = &squares[posx][posy];
            squares[posx - 1][posy].inOpenList = true;
            addToOpen(posx - 1, posy);
        }
    }
    if(!squares[posx][posy + 1].inCloseList && map->isPassable(posx, posy + 1))
    {
        if(squares[posx][posy + 1].inOpenList)
        {
            if(squares[posx][posy].realcost + 1 < squares[posx][posy + 1].realcost)
            {
                squares[posx][posy + 1].realcost = squares[posx][posy].realcost + 1;
                squares[posx][posy + 1].cost = calcCost(posx, posy, toPosx, toPosy);
                squares[posx][posy + 1].before = &squares[posx][posy];
                removeFromOpen(posx, posy + 1);
                addToOpen(posx, posy + 1);
            }
        }
        else
        {
            squares[posx][posy + 1].realcost = squares[posx][posy].realcost + 1;
            squares[posx][posy + 1].cost = calcCost(posx, posy, toPosx, toPosy);
            squares[posx][posy + 1].before = &squares[posx][posy];
            squares[posx][posy + 1].inOpenList = true;
            addToOpen(posx, posy + 1);
        }
    }
    if(!squares[posx][posy - 1].inCloseList && map->isPassable(posx, posy - 1))
    {
        if(squares[posx][posy - 1].inOpenList)
        {
            if(squares[posx][posy].realcost + 1 < squares[posx][posy - 1].realcost)
            {
                squares[posx][posy - 1].realcost = squares[posx][posy].realcost + 1;
                squares[posx][posy - 1].cost = calcCost(posx, posy, toPosx, toPosy);
                squares[posx][posy - 1].before = &squares[posx][posy];
                removeFromOpen(posx, posy - 1);
                addToOpen(posx, posy - 1);
            }
        }
        else
        {
            squares[posx][posy - 1].realcost = squares[posx][posy].realcost + 1;
            squares[posx][posy - 1].cost = calcCost(posx, posy, toPosx, toPosy);
            squares[posx][posy - 1].before = &squares[posx][posy];
            squares[posx][posy - 1].inOpenList = true;
            addToOpen(posx, posy - 1);
        }
    }
};

int CPathFinder::calcCost(int posx, int posy, int toPosx, int toPosy)
{
    return squares[posx][posy].realcost + abs(posx - toPosx) + abs(posy - toPosy);
};

bool CPathFinder::removeFromOpen(int posx, int posy)
{
    auto itr = openList.begin();
    while(itr != openList.end())
    {
        if(*itr == &squares[posx][posy]) break;
        itr++;
    }
    if(itr == openList.end()) return false;
    openList.erase(itr);
    return true;
};

void CPathFinder::addToOpen(int posx, int posy)
{
    auto itr = openList.begin();
    while(itr != openList.end())
    {
        if((*itr)->cost > squares[posx][posy].cost)
        {
            openList.insert(itr, &squares[posx][posy]);
            return;
        }
        ++itr;
    }   
    openList.push_back(&squares[posx][posy]);
};

std::list<point>* LOSfinder::calculateVisisble(std::list<point>* retlist, int posx, int posy, int posz)
{
    //auto retlist = new std::list<point>;
    clearLOS();
    //for(int level = 0; level < 
    point p = {posx, posy, posz};
    retlist->push_back(p);

    p.posx = posx + 1;
    p.posy = posy;
    worklist.push_back(p);

    p.posx = posx + 1;
    p.posy = posy;
    retlist->push_back(p);

    p.posx = posx + 1;
    p.posy = posy + 1;
    worklist.push_back(p);

    p.posx = posx + 1;
    p.posy = posy + 1;
    retlist->push_back(p);

    p.posx = posx;
    p.posy = posy + 1;
    worklist.push_back(p);

    p.posx = posx;
    p.posy = posy + 1;
    retlist->push_back(p);

    p.posx = posx - 1;
    p.posy = posy + 1;
    worklist.push_back(p);

    p.posx = posx - 1;
    p.posy = posy + 1;
    retlist->push_back(p);

    p.posx = posx - 1;
    p.posy = posy;
    worklist.push_back(p);

    p.posx = posx - 1;
    p.posy = posy;
    retlist->push_back(p);
    
    p.posx = posx - 1;
    p.posy = posy - 1;
    worklist.push_back(p);

    p.posx = posx - 1;
    p.posy = posy - 1;    
    retlist->push_back(p);

    p.posx = posx;
    p.posy = posy - 1;
    worklist.push_back(p);

    p.posx = posx;
    p.posy = posy - 1;  
    retlist->push_back(p);

    p.posx = posx + 1;
    p.posy = posy - 1;
    worklist.push_back(p);

    p.posx = posx + 1;
    p.posy = posy - 1; 
    retlist->push_back(p);

    auto itr = worklist.begin();
    while(itr != worklist.end())
    {
        if(map->isVisible(itr->posx, itr->posy, itr->posz) 
            && itr->posx != posx - sizeHsq && itr->posx != posx + sizeHsq
            && itr->posy != posy - sizeWsq && itr->posy != posy + sizeWsq
            && !(itr->posy <= 0 || itr->posy >= (sizeHmap - 1) || itr->posx <= 0 || itr->posx >= sizeWmap - 1))
            if(abs(itr->posx - posx) > abs(itr->posy - posy)) 
            if(itr->posx > posx)
            {
                p.posx = itr->posx + 1;
                p.posy = itr->posy;
                worklist.push_back(p);
                p.posx = itr->posx + 1;
                p.posy = itr->posy;
                retlist->push_back(p);
            }
            else 
            {
                p.posx = itr->posx - 1;
                p.posy = itr->posy;
                worklist.push_back(p);
                p.posx = itr->posx - 1;
                p.posy = itr->posy;
                retlist->push_back(p);
            }
            else if(abs(itr->posx - posx) < abs(itr->posy - posy)) 
            if(itr->posy > posy) 
            {
                p.posx = itr->posx;
                p.posy = itr->posy + 1;
                worklist.push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy + 1;
                retlist->push_back(p);
            }
            else 
            {
                p.posx = itr->posx;
                p.posy = itr->posy - 1;
                worklist.push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy - 1;
                retlist->push_back(p);
            }
            else
            if(itr->posx > posx)
            {
                if(itr->posy > posy)
                {
                p.posx = itr->posx + 1;
                p.posy = itr->posy + 1;
                worklist.push_back(p);
                p.posx = itr->posx + 1;
                p.posy = itr->posy + 1;
                retlist->push_back(p);
                p.posx = itr->posx + 1;
                p.posy = itr->posy;
                worklist.push_back(p);
                p.posx = itr->posx + 1;
                p.posy = itr->posy;
                retlist->push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy + 1;
                worklist.push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy + 1;
                retlist->push_back(p);
                }
                else
                {
                p.posx = itr->posx + 1;
                p.posy = itr->posy - 1;
                worklist.push_back(p);
                p.posx = itr->posx + 1;
                p.posy = itr->posy - 1;
                retlist->push_back(p);
                p.posx = itr->posx + 1;
                p.posy = itr->posy;
                worklist.push_back(p);
                p.posx = itr->posx + 1;
                p.posy = itr->posy;
                retlist->push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy - 1;
                worklist.push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy - 1;
                retlist->push_back(p);
                }

            }
            else
            {
                if(itr->posy > posy)
                {
                p.posx = itr->posx - 1;
                p.posy = itr->posy + 1;
                worklist.push_back(p);
                p.posx = itr->posx - 1;
                p.posy = itr->posy + 1;
                retlist->push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy + 1;
                worklist.push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy + 1;
                retlist->push_back(p);
                p.posx = itr->posx - 1;
                p.posy = itr->posy;
                worklist.push_back(p);
                p.posx = itr->posx - 1;
                p.posy = itr->posy;
                retlist->push_back(p);
                }
                else
                {
                p.posx = itr->posx - 1;
                p.posy = itr->posy - 1;
                worklist.push_back(p);
                p.posx = itr->posx - 1;
                p.posy = itr->posy - 1;
                retlist->push_back(p);
                p.posx = itr->posx - 1;
                p.posy = itr->posy;
                worklist.push_back(p);
                p.posx = itr->posx - 1;
                p.posy = itr->posy;
                retlist->push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy - 1;
                worklist.push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy - 1;
                retlist->push_back(p);
                }
            }

        worklist.erase(itr++);
    };
    if (retlist->begin()->posz == 0)
        return retlist;
    
    std::list<point> z_list;
    for (auto it = retlist->begin(); it != retlist->end(); ++it)
    {
        point p = *it;
        p.posz -= 1;
        z_list.push_back(p);
    }
    retlist->splice(retlist->begin(), z_list);
    return retlist;
};

void LOSfinder::clearLOS()
{
    worklist.clear();
};
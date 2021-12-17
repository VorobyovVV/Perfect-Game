#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "Player.h"

class GameState
{  
public:
    enum class Block { Background, Ground, Count};
private:
    std::vector<std::vector<Block> > _map;
    GameIdx _rows;
    GameIdx _cols;
    ///TODO: map of Player objects?
    //std::unordered_map<std::string, std::pair<GameIdx, GameIdx> > _players;
    std::unordered_map<std::string, Player> _players;
public:
    GameState(std::string const& config_file_name);
    GameState();

    bool serialize(char* buffer, size_t& sz);
    bool deserialize(char const* buffer, size_t const kSize);
    void addPlayer( std::string const& name,
                    std::shared_ptr<UdpSocket> sock,
                    GameIdx x,
                    GameIdx y);
    Player* getPlayer(std::string const& name);
    auto& getListOfPlayers() { return _players; }
    auto& getMap() { return _map; }
    GameIdx getRow() { return _rows; }
    GameIdx getCol() { return _cols; }
    void incrementAll();
    void sendAll();
    void updateMap(GameIdx x, GameIdx y, Block block);
    void updatePos(std::string PlayerName, size_t sizeOfBlock,size_t x , size_t y);

};
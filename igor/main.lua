-- Help the Christmas elves fetch presents in a magical labyrinth!
local parse
do
    local function parse_tile(s)
        local ret = {}
        local i = 1
        local dir = {"up", "right", "down", "left"}
        for c in s:gmatch(".") do
            ret[dir[i]] = c == "1"
            i = i + 1
        end
        return ret
    end

    local function parse_labyrinth()
        local ret = {}
        for _ = 1, 7 do
            local line = {}
            for tile in io.read():gmatch("%S+") do
                table.insert(line, parse_tile(tile))
            end
            table.insert(ret, line)
        end
        return ret
    end

    local function parse_player()
        local ret = { pos = {} }
        ret.n_cards, ret.pos.x, ret.pos.y, ret.tile = io.read():match("(%S+) (%S+) (%S+) (%S+)")
        ret.n_cards = tonumber(ret.n_cards)
        ret.pos.x, ret.pos.y = tonumber(ret.pos.x)+1, tonumber(ret.pos.y)+1
        ret.tile = parse_tile(ret.tile)
        return ret
    end

    local function parse_item()
        local ret = {}
        ret.name, ret.x, ret.y, ret.p_id = io.read():match("(%S+) (%S+) (%S+) (%S+)")
        ret.x, ret.y, ret.p_id = tonumber(ret.x)+1, tonumber(ret.y)+1, tonumber(ret.p_id)
        return ret
    end
    local function parse_items()
        local nb = tonumber(io.read()) -- the total number of items available on board and on player tiles
        local ret = {}
        for _ = 1, nb do
            local item = parse_item()
            ret[item.name] = item
        end
        return ret
    end

    local function parse_quest()
        local ret = {}
        ret.item, ret.p_id = io.read():match("(%S+) (%S+)")
        ret.p_id = tonumber(ret.p_id)
        return ret
    end
    local function parse_quests()
        local nb = tonumber(io.read()) -- the total number of revealed quests for both players
        local ret = {}
        for _ = 1, nb do
            table.insert(ret, parse_quest())
        end
        return ret
    end

    local function raw_parse()
        local ret = {
            turn_type = tonumber(io.read()),
            map = parse_labyrinth(),
            players = {
                me = parse_player(),
                opponent = parse_player(),
            },
            items = parse_items(),
            quests = parse_quests(),
        }
        return ret
    end

    local function build_map(world)
        for _, quest in ipairs(world.quests) do
            quest.item = world.items[quest.item]
            quest.item.p_id = quest.p_id
        end

        for _, item in pairs(world.items) do
            io.stderr:write(item.x.." "..item.y.."\n")
            local tile
            if item.x == -1 then
                tile = world.players.me.tile
            elseif item.y == -2 then
                tile = world.players.opponent.tile
            else
                tile = world.map[item.x][item.y]
            end
            tile.p_id = item.p_id
        end

        local me_quests = {}
        local opponent_quests = {}
        for _, quest in ipairs(world.quests) do
            if quest.p_id == 0 then
                table.insert(me_quests, quest.item)
            else
                table.insert(opponent_quests, quest.item)
            end
        end
        world.players.me.quests = me_quests
        world.players.opponent.quests = opponent_quests
    end

    parse = function()
        local ret = raw_parse()
        build_map(ret)
        return ret
    end
end

local play
do
    local function play_push(world)
        print("PUSH 3 RIGHT")
    end

    local function calc_distance(p1, p2)
        local dx = p2.x - p1.x
        local dy = p2.y - p1.y
        return dx*dx + dy*dy
    end

    local function accessible_item_paths(map, pos)
        -- TODO
    end

    local function get_closest_quest_item(quests, obj)
        local closest = {
            distance = 7*7+1,
        }
        for _, quest in pairs(quests) do
            local item = quest.item
            local distance = calc_distance(quest.item, obj)
            if distance < closest.distance then
                closest = {
                    item = item,
                    distance = distance,
                }
            end
        end
        return closest.item
    end

    local function play_move(world)
        local moves = {}

        -- Accessible items
        local paths = accessible_item_paths(world.map, world.players.me)
        while #paths > 0 do
            local path = paths[1]
            if #path + #moves > 20 then break end

            for _, move in ipairs(path) do
                table.insert(moves, move)
            end
            paths = accessible_item_paths(world.map, path.target)
        end

        -- TODO Get closer to the next target
    end

    play = function(world)
        io.stderr:write(world.turn_type.." "..type(world.turn_type).."\n")
        if world.turn_type == 0 then
            play_push(world)
        else
            play_move(world)
        end
    end
end

-- game loop
while true do
    local world = parse()
    play(world)

    -- Write an action using print()
    -- To debug: io.stderr:write("Debug message\n")
end

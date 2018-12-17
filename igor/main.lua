-- Help the Christmas elves fetch presents in a magical labyrinth!

local function p_err(...)
    io.stderr:write(table.concat({...}, " ").."\n")
end

-- Table helpers
local tbl = {}
do
    tbl.copy_fields = function(field_list, source)
        local ret = {}
        for _, field in ipairs(field_list) do
            ret[field] = source[field]
        end
        return ret
    end

    tbl.copy = function(t)
        local ret = {}
        for k, v in pairs(t) do
            ret[k] = v
        end
        return ret
    end

    tbl.hash_merge = function(...)
        local ret = {}
        for _, t in ipairs{...} do
            for k, v in pairs(t) do
                ret[k] = v
            end
        end
        return ret
    end

    tbl.tostring = function(t, indent)
        indent = indent or ""
        local ret = {}
        for k, v in pairs(t) do
            table.insert(ret, indent..tostring(k).." = "..tostring(v))
        end
    end

    tbl.define = function ( t, no_revert, custom_mt )
        local data = {}
        for k, v in pairs(t) do
            data[k] = v
            if type(k) == "number" then
                if not no_revert then
                    data[v] = v
                end
            else
                if not no_revert then
                    data[v] = k
                end
            end
        end
        local px = {}
        local mt = {
            __index = function(_, index)
                if not data[index] then error("No index named "..index) end
                return data[index]
            end,
            __newindex = function() error("Read-only table !") end,
            __tostring = function()
                local ret = {}
                for k, v in pairs(data) do
                    table.insert(ret, "- "..tostring(k).." = "..tostring(v))
                end
                return table.concat(ret, "\n")
            end,
            __ipairs = function()
                return ipairs(data)
            end,
        }
        if custom_mt then mt = tbl.hash_merge(mt, custom_mt) end
        setmetatable(px,mt)
        return px
    end
end

-- Defines
local DIR = tbl.define{"UP", "RIGHT", "DOWN", "LEFT"}
--p_err("DIR\n"..tostring(DIR))
local INV_DIR = tbl.define{
    [DIR.UP] = DIR.DOWN,
    [DIR.LEFT] = DIR.RIGHT,
}
--p_err("INV_DIR\n"..tostring(INV_DIR))

-- Parsers
local parse
do
    local function parse_tile(s)
        local ret = {half_dir = {}}
        local i = 1
        for c in s:gmatch(".") do
            ret.half_dir[DIR[i]] = c == "1"
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
        local ret = {}
        ret.n_cards, ret.x, ret.y, ret.tile = io.read():match("(%S+) (%S+) (%S+) (%S+)")
        ret.n_cards = tonumber(ret.n_cards)
        ret.x, ret.y = tonumber(ret.x)+1, tonumber(ret.y)+1
        ret.tile = parse_tile(ret.tile)
        return ret
    end

    local function parse_item()
        local ret = {}
        ret.name, ret.x, ret.y, ret.p_id = io.read():match("(%S+) (%S+) (%S+) (%S+)")
        ret.x, ret.y, ret.p_id = tonumber(ret.x)+1, tonumber(ret.y)+1, tonumber(ret.p_id)
        if ret.x <= 0 then ret.x = ret.x - 1 end
        if ret.y <= 0 then ret.y = ret.y - 1 end
        return ret
    end
    local function parse_items()
        local nb = tonumber(io.read()) -- the total number of items available on board and on player tiles
        local ret = { [0] = {}, [1] = {} }
        for _ = 1, nb do
            local item = parse_item()
            ret[item.p_id][item.name] = item
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

    local function build_world(world)
        for _, quest in ipairs(world.quests) do
            quest.item = world.items[quest.p_id][quest.item]
            quest.item.p_id = quest.p_id
        end

        for _, item_collection in pairs(world.items) do
            for _, item in pairs(item_collection) do
                local tile
                if item.x == -1 then
                    tile = world.players.me.tile
                elseif item.x == -2 then
                    tile = world.players.opponent.tile
                else
                    tile = world.map[item.y][item.x]
                end
                tile.item = item
            end
        end

        local me = world.players.me
        local opponent = world.players.opponent

        local me_quests = {}
        local opponent_quests = {}
        for _, quest in ipairs(world.quests) do
            if quest.p_id == 0 then
                table.insert(me_quests, quest.item)
            else
                table.insert(opponent_quests, quest.item)
            end
        end
        me.quests = me_quests
        opponent.quests = opponent_quests

        local function get_shifted(x, y, dir)
            if dir == DIR.UP then
                y = y - 1
            elseif dir == DIR.DOWN then
                y = y + 1
            elseif dir == DIR.RIGHT then
                x = x + 1
            elseif dir == DIR.LEFT then
                x = x - 1
            else
                error("Unknown dir: "..tostring(dir))
            end
            local row = world.map[y]
            if not row then return nil end
            return row[x]
        end

        local function path_dir_exists(x, y, dir)
            local tile = world.map[y][x]
            if not tile.half_dir[dir] then return false end
            local other_tile = get_shifted(x, y, dir)
            if not other_tile then return false end
            return other_tile.half_dir[INV_DIR[dir]]
        end

        for y = 1, 7 do
            for x = 1, 7 do
                local tile = world.map[y][x]
                for _, dir in ipairs(DIR) do
                    tile[dir] = path_dir_exists(x, y, dir)
                end
            end
        end

        world.map[me.y][me.x].p_id = 0
        world.map[opponent.y][opponent.x].p_id = 1
    end

    local function print_map(map, tile_printer)
        local tile_size = tile_printer(true)
        local bar = ("-"):rep(tile_size*7+6)
        for _, row in ipairs(map) do
            local row_s = {}
            for _, tile in ipairs(row) do
                table.insert(row_s, tile_printer(tile))
            end
            p_err(table.concat(row_s, "|").."\n"..bar)
        end
    end

    local dir_sym = {[DIR.UP] = "^", [DIR.DOWN] = "_", [DIR.RIGHT] = ">", [DIR.LEFT] = "<"}
    local function tile_dir_printer(tile)
        if tile == true then return 4 end
        local ret = ""
        for dir, sym in pairs(dir_sym) do
            if tile[dir] then
                ret = ret..sym
            else
                ret = ret.."-"
            end
        end
        return ret
    end

    local function tile_half_dir_printer(tile)
        if tile == true then return 4 end
        local ret = ""
        for dir, sym in pairs(dir_sym) do
            if tile.half_dir[dir] then
                ret = ret..sym
            else
                ret = ret.."-"
            end
        end
        return ret
    end

    local function tile_obj_printer(tile)
        if tile == true then return 1 end
        if tile.item then
            return "+"..tile.item.p_id
        elseif tile.p_id then
            return " "..tile.p_id
        else
            return " "
        end
    end

    parse = function()
        local ret = raw_parse()
        build_world(ret)
        -- print_map(ret.map, tile_dir_printer)
        -- p_err()
        -- print_map(ret.map, tile_half_dir_printer)
        -- p_err()
        -- print_map(ret.map, tile_obj_printer)
        return ret
    end
end

local play
do
    local function play_push(world)
        -- TODO Careful of the index shift (PUSH 3 <=> map[4])
        print("PUSH 3 RIGHT")
    end

    local function calc_distance(p1, p2)
        local dx = p2.x - p1.x
        local dy = p2.y - p1.y
        return dx*dx + dy*dy
    end

    local function closest_accessible_item_path(map, start_pos)
        local known_tile = {}
        local function add_known_tile(pos)
            if pos.x < 1 or pos.y < 1 or pos.x > 7 or pos.y > 7 then return false end
            local pos_name = pos.x.." "..pos.y
            if known_tile[pos_name] then
                return false
            else
                known_tile[pos_name] = true
                return true
            end
        end

        local heads = {
            tbl.hash_merge(start_pos, map[start_pos.y][start_pos.x], {
                path = {},
            }),
        }
        local dirs = {
            [DIR.UP] = {x = 0, y = -1},
            [DIR.DOWN] = {x = 0, y = 1},
            [DIR.RIGHT] = {x = 1, y = 0},
            [DIR.LEFT] = {x = -1, y = 0},
        }
        while #heads > 0 do
            local new_heads = {}
            for _, head in ipairs(heads) do
                local tile = map[head.y][head.x]
                local item = tile.item
                if item and item.p_id == 0 then
                    local path = head.path
                    path.target = {x = head.x, y = head.y}
                    return head.path
                end

                for dir_name, dir in pairs(dirs) do
                    if tile[dir_name] then
                        -- p_err("dir_name:", dir_name)
                        local tgt_pos = {x = head.x+dir.x, y = head.y+dir.y}
                        if add_known_tile(tgt_pos) then
                            local path = tbl.copy(head.path)
                            table.insert(path, dir_name)
                            local access = tbl.copy_fields(DIR, map[tgt_pos.y][tgt_pos.x])
                            access[INV_DIR[dir_name]] = nil
                            -- p_err("new head:", tbl.tostring(access))
                            table.insert(new_heads, tbl.hash_merge(tgt_pos, {
                                path = path,
                            }))
                        end
                    end
                end
            end
            heads = new_heads
            p_err("heads:"..#heads)
        end
        return false
    end

    local function play_move(world)
        local moves = {}

        -- Accessible items
        local path = closest_accessible_item_path(world.map, world.players.me)
        while path do
            if #path + #moves > 20 then break end

            for _, move in ipairs(path) do
                table.insert(moves, move)
            end
            world.map[path.target.y][path.target.x].item = nil
            path = closest_accessible_item_path(world.map, path.target)
        end

        -- TODO Get closer to the next target

        if #moves > 0 then
            print("MOVE "..table.concat(moves, " "))
        else
            print("PASS")
        end
    end

    play = function(world)
        if world.turn_type == 0 then
            play_push(world)
        else
            play_move(world)
        end
    end
end

-- game loop
while true do
    local start = os.clock()
    local world = parse()
    p_err("parse delay: "..(os.clock()-start))

    start = os.clock()
    play(world)
    p_err("play delay: "..(os.clock()-start))

    -- Write an action using print()
    -- To debug: io.stderr:write("Debug message\n")
end

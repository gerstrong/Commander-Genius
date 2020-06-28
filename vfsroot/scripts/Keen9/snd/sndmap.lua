-- Ampton becomes a Yeti
local M = {}
local M_mt = { __index = M }

function M:new(start)
    if self ~= M then
    	return nil, "First argument must be self"
    end

    local o = setmetatable({}, M_mt)
    o._count = start
    return o
end
setmetatable(M, { __call = M.new })



function M:getSoundMapping(action)
-- Number of sound to be used for given action
    local sndMap = 
    { 
      [29] = 0x01, 
      [80] = 0x1C,
      [82] = 0x1C,
      [90] = 0x1C,
      [85] = 0x0,
      [86] = 0x0
    }

-- Check if entry was found, return -1 otherwise    
    if sndMap[action] then
        return sndMap[action]
    else
        return -1;
    end
end

return M


-- Shockshound becomes Mortimer McMire
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


function M_mt:__tostring()
    return string.format("%d", self._count)
end

function M:isStunnableWithPogo()
-- Can Keen stun the enemy with pogo alone?
    return true
end


function M:isInvincible()
-- Shots never kill it
    return true
end

function M:canRecoverFromStun()
    return true
end

function M:healthPoints()
-- Number of hit points before enemy is stunned
    return 30
end

function M:turnAroundOnCliff()
-- Shots never kill it
    return false
end

function M:endGameOnDefeat()
-- Shots never kill it
    return true
end


return M


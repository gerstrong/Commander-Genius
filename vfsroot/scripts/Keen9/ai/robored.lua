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


function M_mt:__tostring()
    return string.format("%d", self._count)
end

function M:alternateShoot()
-- Can Keen stun the enemy with pogo alone?
    return true
end

function M:isInvincible()
-- Shots never kill it
    return true
end

function M:willNeverStop()
-- Robo red will continue chasing and shoot wherever required
    return true
end

function M:mayJiggle()
-- Robo red will continue chasing and shoot wherever required
    return false
end


return M


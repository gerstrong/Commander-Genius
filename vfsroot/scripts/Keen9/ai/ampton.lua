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


function M:healthPoints()
-- Number of hit points before enemy is stunned
    return 3
end

function M:mayShoot()
-- Enemy may shoot
    return true
end

function M:screamAfterShoot()
-- Does enemy screan after a shot?
    return false
end

function M:allowClimbing()
-- Can Enemy climp up poles?
    return false
end


function M:walkSound()
-- Walk sound of the enemy
    return 0
end


return M


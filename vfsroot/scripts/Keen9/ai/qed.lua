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

function M:getLevelText(level)
       if level == 5 then
          text =  "Princess Lindsey says:\n"
          text += "You won't be able to \n"
          text += "defeat Mortimer with\n"
          text += "your stunner alone!"
       elseif level == 11 then
          text =  "Lt. Barker says:\n"
          text += "Beware of the\n"
          text += "Krodacian Overlords!"
       elseif level == 2 then
          text =  "Spot says:\n"
          text += "The Shikadi are\n"
          text += "invincible while\n"
          text += "climbing poles!" 
       elseif level == 15 then
          text =  ""
       else:
          text =  "Oracle Janitor says:\n"
          text += "There's something\n"
          text += "mysterious beneath the\n"
          text += "giant crater."
       return text
end


function M:getLevelTextBmp(level)
       if level == 5 then
          bmpIdx = 40
       elseif level == 11 then
          bmpIdx = 38
       elseif level == 2 then
          bmpIdx = 41
       else:
          bmpIdx = 39
       return bmpIdx
end


return M


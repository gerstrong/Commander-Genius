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
      text = text .. "You won't be able to \n"
      text = text .. "defeat Mortimer with\n"
      text = text .. "your stunner alone!"
  	elseif level == 11 then
      text =  "Lt. Barker says:\n"
      text = text .. "Beware of the\n"
      text = text .. "Krodacian Overlords!"
  	elseif level == 2 then
      text =  "Spot says:\n"
      text = text .. "The Shikadi are\n"
      text = text .. "invincible while\n"
      text = text .. "climbing poles!"   	
  	elseif level == 15 then
      text = ""  	
    else
      text =  "Oracle Janitor says:\n"
      text = text .. "There's something\n"
      text = text .. "mysterious beneath the\n"
      text = text .. "giant crater."
    end
    return text
end

function M:getLevelTextBmp(level)
    if level == 5 then
      bmpIdx = 40
    elseif level == 11 then
      bmpIdx = 38
    elseif level == 2 then
      bmpIdx = 41
    else
      bmpIdx = 39
    end
    return bmpIdx
end


return M


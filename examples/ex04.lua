local ej = require "ejoy2d"
local fw = require "ejoy2d.framework"
local pack = require "ejoy2d.simplepackage"
local sprite = require "ejoy2d.sprite"

pack.load {
	pattern = fw.WorkDir..[[examples/asset/?]],
	"sample",
}

local scissor = false
local obj = ej.sprite("sample","mine")
obj.resource.frame = 70
obj.label.text = "Hello World"
obj:ps(500,300)

local game = {}

function game.update()
	obj.frame = obj.frame + 1
end

function game.drawframe()
	obj:draw()
end

function game.touch(what, x, y)
	if what == "END" then
		local touched = obj:test(pos,x,y)
		if touched then
			if touched.name == "label" then
				touched.text = "label touched"
			end
			if touched.name == "panel" then
				scissor = not scissor
				touched.scissor = scissor
				obj.label.text = scissor and "Set scissor" or "Clear scissor"
			end
		else
			obj.label.text = "Not Hit"
		end
	end
end

ej.start(game)



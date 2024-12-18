local resources = require 'resources'

local keys = {
    shapes = {
        ONE_WAY_PLATFORM = 'one-way-platform'
    },
    shapeGraphics = {
        PLATFORM = 'platform'
    }
}

---@class PlatformCreationOptions
---@field length integer? default = 3
---@field bodyType integer? default sol.BodyType.STATIC
---@field shapeKey string? default keys.shapes.ONE_WAY_PLATFORM
---@field script string?
---@field scriptArgument any?
---@field layer string?

---@param scene sol.Scene
---@param position sol.Point
---@param options PlatformCreationOptions?
---@return sol.Body
local function createPlatform(scene, position, options)
    if not options then
        options = {}
    end
    if options.length == nil then
        options.length = 3
    elseif options.length < 2 then
        error('Platform length cannot be less than 2, ' .. options.length .. ' requested')
    end
    if options.bodyType == nil then
        options.bodyType = sol.BodyType.STATIC
    end
    if not options.shapeKey then
        options.shapeKey = keys.shapes.ONE_WAY_PLATFORM
    end
    local SPRITE_WIDTH = 128
    local sprite_sheet = resources.getSpriteSheet(resources.keys.spriteSheets.PLATFORM)
    local sprites = {
        {
            sprite = { spriteSheet = sprite_sheet, spriteIndex = 0 },
            position = { x = 0, y = 0 }
        }
    }
    for i = 1, options.length - 2 do
        table.insert(
            sprites,
            {
                sprite = { spriteSheet = sprite_sheet, spriteIndex = 1 },
                position = { x = i * SPRITE_WIDTH, y = 0 }
            }
        )
    end
    table.insert(
        sprites,
        {
            sprite = { spriteSheet = sprite_sheet, spriteIndex = 2 },
            position = { x = (options.length - 1) * SPRITE_WIDTH, y = 0 }
        }
    )
    local body = scene:createBody(
        position,
        {
            type = options.bodyType,
            shapes = {
                [options.shapeKey] = {
                    type = sol.BodyShapeType.POLYGON,
                    physics = {
                        isPreSolveEnabled = true,
                        restitution = 0.2,
                        density = 100
                    },
                    rect = { x = 0, y = 0, w = options.length * SPRITE_WIDTH, h = 64 },
                    graphics = {
                        [keys.shapeGraphics.PLATFORM] = {
                            animationIterations = 0,
                            frames = { { sprites = sprites } }
                        }
                    }
                }
            }
        },
        options.script,
        options.scriptArgument
    )
    local shape = body:getShape(options.shapeKey)
    if shape then
        shape:setCurrentGraphics(keys.shapeGraphics.PLATFORM)
    end
    if options.layer then
        body:setLayer(options.layer)
    end
    return body
end

local module = {
    new = createPlatform,
    keys = keys
}
module.__index = module
return setmetatable({}, module)

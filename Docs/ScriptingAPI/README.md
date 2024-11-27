# Scripting API

Scripting in XEN is done in Lua and XEN provides an API for Lua that can be used to write anything
from
simple initialization code to complex frame-by-frame behavior.

Any [GameObject]() can have a script associated with it by adding the [Behavior]() component.

## A Basic Script

```lua
function onAwake(go)
    -- Initialization code goes here
end

function onUpdate(go, dT)
    -- Update logic goes here
end

function onDestroyed(go)
    -- Cleanup code goes here
end 
```

## Event Hooks

### `onAwake(go)`

- `go`: GameObject

### `onUpdate(go, dT)`

- `go`: GameObject
- `dT`: Float

### `onDestroyed(go)`

- `go`: GameObject

### `onMouseMove(go, mouseX, mouseY)`

- `go`: GameObject
- `mouseX`: Float
- `mouseY`: Float

### `onCollider(go, col)`

- `go`: GameObject
- `col`: GameObject

### `onTriggerEnter(go, trig)`

- `go`: GameObject
- `trig`: GameObject

### `onTriggerExit(go, trig)`

- `go`: GameObject
- `trig`: GameObject

## GameObjects

[GameObject API]()

## Transforms

A GameObject can be moved around the scene by manipulating its [Transform]() component. For example,
if we wanted to move our GameObject **2 units per frame**:

[Transform API]()

```lua
function onUpdate(go, dT)
    local transform = go:GetTransform()
    transform.Translate(-2 * dT, 0)
end
```

## Input

A global `InputManager` object is provided to all Lua scripts and can be used to get
user input.

[Input API]()

```lua
function onUpdate(go, dT)
    local transform = go:GetTransform()
    -- Move our game object by two units when the Up key is pressed
    if (InputManager:GetKeyDown(KEY_UP)) then
        transform.Translate(-2 * dT, 0)
    end
end
```
-- Variables

moveSpeed = 4

-- Event Hooks

function onAwake(go)
    local transform = go:GetTransform()
    transform:SetPosition(-42, 0)
end

function onUpdate(go, dT)
    local transform = go:GetTransform()

    if (InputManager:GetKeyDown(KEY_UP)) then
        transform:Translate(0, -moveSpeed * dT)
    end

    if (InputManager:GetKeyDown(KEY_DOWN)) then
        transform:Translate(0, moveSpeed * dT)
    end
end

function onDestroyed(go)
end
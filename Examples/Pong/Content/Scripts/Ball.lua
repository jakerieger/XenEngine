function onAwake(go)
end

function onUpdate(go, dT)
    -- Put game loop code here
    local transform = go:GetTransform()
    transform:Translate(-4.0 * dT, 0)
end

function onDestroyed(go)
end
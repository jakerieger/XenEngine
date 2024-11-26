function onAwake(go)
    -- Put initialization code here
    print(string.format("GameObject<%s> created.", go:GetName()))
end

function onUpdate(go, dT)
    -- Put game loop code here
    local transform = go:GetTransform()
    transform:Translate(-4.0 * dT, 0)
end

function onDestroyed(go)
    -- Put cleanup code here
    print(string.format("GameObject<%s> destroyed.", go:GetName()))
end
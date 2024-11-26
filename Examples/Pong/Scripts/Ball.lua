function onAwake(go)
    -- Put initialization code here
    print(string.format("GameObject<%s> created.", go:GetName()))
end

function onUpdate(go, dT)
    -- Put game loop code here
end

function onDestroyed(go)
    -- Put cleanup code here
    print(string.format("GameObject<%s> destroyed.", go:GetName()))
end
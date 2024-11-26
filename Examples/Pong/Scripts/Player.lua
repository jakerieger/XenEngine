function onAwake(go)
    -- Put initialization code here
    print(string.format("GameObject<%s> created.", go:GetName()))
end

function onUpdate(go, dT)
    local transform = go:GetTransform()

    if (GetKey(KEY_UP)) then
        transform:Translate(0, 2.0 * dT)
    end

    if (GetKey(KEY_DOWN)) then
        transform:Translate(0, -2.0 * dT)
    end
end

function onDestroyed(go)
    -- Put cleanup code here
    print(string.format("GameObject<%s> destroyed.", go:GetName()))
end
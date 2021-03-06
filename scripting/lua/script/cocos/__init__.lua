cc = cc or {}

-- cc.log
cc.log = function(...)
    print(string.format(...))
end

-- for CCLuaEngine traceback
function __G__TRACKBACK__(msg)
    cc.log("----------------------------------------")
    cc.log("LUA ERROR: " .. tostring(msg) .. "\n")
    cc.log(debug.traceback())
    cc.log("----------------------------------------")
end

-- load lua file under a folder, include subfolders
function loadLua(name)
    -- internal and external path
    local internalPath
    if CCDevice:getPlatform() == cc.PLATFORM_ANDROID then
        internalPath = "script" .. "/" .. name
    else
        internalPath = CCFileUtils:sharedFileUtils():fullPathForFilename("script") .. "/" .. name
    end
    local externalPath = CCUtils:externalize("script")  .. "/" .. name
    
    -- search file in internal and external, exclude duplicated entry
    local entries = {}
    local internalEntries = {}
    if CCDevice:getPlatform() == cc.PLATFORM_ANDROID then
        internalEntries = CCFileUtils:sharedFileUtils():listAssets(internalPath)
    else
        internalEntries = lfs.dir(internalPath)
    end
    for entry in internalEntries do
        local isLua = entry ~= "__init__.lua" and string.find(entry, ".lua") ~= nil
        local isLc = entry ~= "__init__.lc" and string.find(entry, ".lc") ~= nil
        if entry ~= "." and entry ~= ".." and (isLua or isLc) then
            local s, n = string.gsub(entry, "%.lua+", "")
            s, n = string.gsub(s, "%.lc+", "")
            entries[tostring(s)] = internalPath
        end
    end
    if CCUtils:isPathExistent(externalPath) then
        for entry in lfs.dir(externalPath) do
            local isLua = entry ~= "__init__.lua" and string.find(entry, ".lua") ~= nil
            local isLc = entry ~= "__init__.lc" and string.find(entry, ".lc") ~= nil
            if entry ~= "." and entry ~= ".." and (isLua or isLc) then
                local s, n = string.gsub(entry, "%.lua+", "")
                s, n = string.gsub(s, "%.lc+", "")
                entries[tostring(s)] = externalPath
            end
        end
    end
    
    -- load
    for file,path in pairs(entries) do
        local fullpath = path .. "/" .. file
        require(fullpath)
    end
end

-- load all cocos core lua
loadLua("cocos")
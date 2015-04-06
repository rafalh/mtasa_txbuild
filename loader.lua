local code = __LUA_CHUNK_TBL__
local i = 0
local function reader()
	i = i + 1
	return code[i]
end

local func, err = f.load(reader)
if(func) then
	func()
else
	outputDebugString('Failed to load '..getResourceName(resource)..': '..err, 1)
end

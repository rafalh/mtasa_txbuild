-- Global variables
__OUTPUT__ = {}
__FILE__, __LINE__ = '', 1
local g_IncludeDirs = {''}
local g_IncludeOnce = {}
local g_Defines = {}

-- Preprocessor internal functions

local function preprocessFile(file)
	local codeTbl = {}
	local lineNr = 1
	
	local definesStr = ''
	for k, v in pairs(g_Defines) do
		definesStr = definesStr..('%s=%q '):format(k, v)
		
	end
	table.insert(codeTbl, definesStr)
	
	for line in file:lines() do
		local codeLine
		local hashPos = line:match('^%s*()#')
		if(hashPos) then
			codeLine = line:sub(hashPos + 1):gsub('%-%-[^\n]*', '')
			
			local codeAfterEnd = codeLine:match('^%s*end%s*(.*)')
			if(not codeAfterEnd) then
				codeLine = '__LINE__='..lineNr..' '..codeLine
			elseif(codeAfterEnd:match('%w')) then
				print("["..__FILE__..":"..lineNr.."] Error! Unexpected code after \"end\": "..codeAfterEnd)
				os.exit(-1)
			end
		else
			--local lineQuoted = ('%q'):format(line)
			--lineQuoted = lineQuoted:gsub('$(%b())', '"..%1.."')
			
			local i = 1
			local tbl = {}
			while(true) do
				local pos = line:find('$', i, true)
				if(pos) then
					if(pos > 1) then
						local str = ('%q'):format(line:sub(i, pos - 1))
						table.insert(tbl, str)
					end
					
					local arg, endPos = line:match('^$(%b())()', pos)
					if(not arg) then
						arg, endPos = line:match('^$([%w_]+%b())()', pos)
					end
					if(not arg) then
						arg, endPos = line:match('^$([%w_]+)()', pos)
					end
					if(arg) then
						table.insert(tbl, 'tostring('..arg..')')
					else
						table.insert(tbl, '\'$\'')
						endPos = pos + 1
					end
					
					i = endPos
				else
					if(i <= line:len()) then
						local str = ('%q'):format(line:sub(i))
						table.insert(tbl, str)
					end
					break
				end
			end
			
			if(#tbl > 0) then
				local str = table.concat(tbl, '..')
				codeLine = '__LINE__='..lineNr..' __OUTPUT__[__LINE__]=(__OUTPUT__[__LINE__] or "")..'..str
			else
				codeLine = '__LINE__='..lineNr
			end
		end
		table.insert(codeTbl, codeLine)
		lineNr = lineNr + 1
	end
	
	local linesCount = lineNr
	local code = table.concat(codeTbl, '\n')
	--print(code)
	
	local func, err = loadstring(code, 'luapp')
	if(not func) then
		code = code:sub(1, 16000)
		print('Error! Failed to parse '..__FILE__..':\n'..err..'\nCode:\n'..code)
		os.exit(-1)
	end
	
	local oldOutput = __OUTPUT__
	__OUTPUT__ = {}
	local status, err = pcall(func)
	if(not status) then
		code = code:sub(1, 16000)
		print('Error! Failed to preprocess '..__FILE__..':\n'..err..'\nCode:\n'..code)
		os.exit(-1)
	end
	
	for i = 1, linesCount do
		if(not __OUTPUT__[i]) then
			__OUTPUT__[i] = ''
		end
	end
	
	local result = table.concat(__OUTPUT__, '\n')
	__OUTPUT__ = oldOutput
	return result
end

local function preprocess(path)
	local fullPath, file
	for i, incDir in ipairs(g_IncludeDirs) do
		file = io.open(incDir..path, 'r')
		if(file) then
			fullPath = incDir..path
			break
		end
	end
	
	if(not file) then
		return false
	end
	
	local dir = fullPath:gsub('[^/\\]+$', '')
	local oldFile, oldLine = __FILE__, __LINE__
	table.insert(g_IncludeDirs, dir)
	__FILE__ = fullPath
	__LINE__ = 1
	
	local buf = preprocessFile(file, true)
	
	table.remove(g_IncludeDirs)
	__FILE__ = oldFile
	__LINE__ = oldLine
	
	file:close()
	return buf
end

local function handleArgs(arg)
	local outputPath = 'output.lua'
	local sourceList = {}
	local i = 1
	while(i <= #arg) do
		local opt = arg[i]:sub(1, 2)
		if(opt == '-o') then
			if(arg[i]:len() > 2) then
				outputPath = arg[i]:sub(3)
			elseif(i + 1 <= #arg) then
				outputPath = arg[i+1]
				i = i + 1
			end
		elseif(opt == '-I') then
			if(arg[i]:len() > 2) then
				table.insert(g_IncludeDirs, arg[i]:sub(3)..'/')
			elseif(i + 1 <= #arg) then
				table.insert(g_IncludeDirs, arg[i+1]..'/')
				i = i + 1
			end
		elseif(opt == '-D') then
			local def
			if(arg[i]:len() > 2) then
				def = arg[i]:sub(3)
			elseif(i + 1 <= #arg) then
				def = arg[i+1]
				i = i + 1
			end
			
			local name, val = def:match('(%w+)=(.*)')
			if(name and val) then
				g_Defines[name] = val
			end
		else
			table.insert(sourceList, arg[i])
		end
		i = i + 1
	end
	return outputPath, sourceList
end

local function main(arg)
	-- Handle args table
	outputPath, sourceList = handleArgs(arg)
	
	-- Check if arguments are valid
	if(not outputPath or #sourceList < 1) then
		print("Error! Invalid arguments count.")
		os.exit(-1)
	end
	
	local result = {}
	
	-- Process all files
	for i, path in ipairs(sourceList) do
		local dir = path:gsub('[^/\\]+$', '')
		table.insert(g_IncludeDirs, dir)
		__FILE__ = path
		__LINE__ = 1
		
		-- Open LUA file
		local inputFile = io.open(path, 'r')
		if(inputFile) then
			-- Preprocess file
			local data = preprocessFile(inputFile)
			
			-- Save result
			table.insert(result, data)
			
			-- Close file
			inputFile:close()
		else
			print("Error! Cannot open \""..path.."\".")
			os.exit(-1)
		end
		table.remove(g_IncludeDirs)
	end
	
	-- Save preprocessed code in output file
	local outputFile = io.open(outputPath, 'w')
	if(not outputFile) then
		print("Error! Cannot open \""..outputPath.."\".")
		os.exit(-1)
	end
	if(#sourceList == 1) then
		outputFile:write(result[1])
	else
		outputFile:write('do '..table.concat(result, ' end do ')..' end')
	end
	outputFile:close()
end

-- Utility functions

function include(path)
	local buf = preprocess(path)
	
	if(not buf) then
		print("["..__FILE__..":"..__LINE__.."] Error! Cannot include \""..path.."\" (dirs "..table.concat(g_IncludeDirs, ', ')..").")
		os.exit(-1)
	end
	
	buf = buf:gsub('%-%-%[%[.-%]%]', ''):gsub('%-%-[^\n]*', ''):gsub('%s+', ' ')
	__OUTPUT__[__LINE__] = buf
	return true
end

function includeGuard()
	if(g_IncludeOnce[__FILE__]) then
		return true
	end
	g_IncludeOnce[__FILE__] = true
	return false
end

function load(path)
	__LOAD__ = true
	
	local buf = preprocess(path)
	if(not buf) then
		print("["..__FILE__..":"..__LINE__.."] Error! Cannot load \""..path.."\".")
		os.exit(-1)
	end
	
	local func, err = loadstring(buf, path)
	if(not func) then
		print("["..__FILE__..":"..__LINE__.."] Error! Cannot load \""..path.."\": "..err.."\nCode:\n"..buf)
		os.exit(-1)
	end
	
	local status, err = pcall(func)
	if(not status) then
		print("["..__FILE__..":"..__LINE__.."] Error! Cannot load \""..path.."\": "..err.."\nCode:\n"..buf)
		os.exit(-1)
	end
	
	__LOAD__ = false
end

function isLoaded()
	return __LOAD__
end

main(arg)

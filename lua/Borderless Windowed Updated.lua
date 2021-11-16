FullscreenWindowed = FullscreenWindowed or {}

FullscreenWindowed.mod_path = ModPath
FullscreenWindowed.save_path = SavePath .. "FullscreenWindowed.json"
_, FullscreenWindowed.library = blt.load_native(FullscreenWindowed.mod_path .. "Borderless Windowed Updated.dll")

FullscreenWindowed._settings = {
	display_mode = 0
}

function FullscreenWindowed:save_settings()
	local file = io.open(self.save_path, "w+")
	if file then
		file:write(json.encode(self._settings))
		file:close()
	end
end

function FullscreenWindowed:load_settings()
	local file = io.open(self.save_path, "r")
	if file then
		for k, v in pairs(json.decode(file:read("*all")) or {}) do
			self._settings[k] = v
		end
		file:close()
	end
end

Hooks:PostHook(__classes["Application"], "apply_render_settings", "FullscreenWindowedApplyRenderSettings", function(self)
	FullscreenWindowed.library.change_display_mode(FullscreenWindowed._settings.display_mode, RenderSettings.resolution.x, RenderSettings.resolution.y, RenderSettings.adapter_index)
end)

Hooks:PostHook(Setup, "init_managers", "FullscreenWindowedInit", function(self, managers)
	if io.file_is_readable(FullscreenWindowed.save_path) then
		FullscreenWindowed:load_settings()
		FullscreenWindowed.library.change_display_mode(FullscreenWindowed._settings.display_mode, RenderSettings.resolution.x, RenderSettings.resolution.y, RenderSettings.adapter_index)
	else
		FullscreenWindowed._settings.display_mode = managers.viewport:is_fullscreen() and 0 or 1
	end
end)

Hooks:PostHook(MenuOptionInitiator, "modify_video", "FullscreenWindowedDisplayMode", function(self, node)
	local adapter_item = node:item("choose_video_adapter")
	if adapter_item then
		adapter_item:set_enabled(FullscreenWindowed._settings.display_mode ~= 1)
	end

	local br_item = node:item("brightness")
	if br_item then
		br_item:set_enabled(FullscreenWindowed._settings.display_mode == 0)
	end

	function MenuCallbackHandler:on_change_display_mode(dm_item)
		local choice = dm_item:value()

		if FullscreenWindowed._settings.display_mode == choice then
			return
		end

		managers.viewport:set_fullscreen(choice == 0)
		FullscreenWindowed.library.change_display_mode(choice, RenderSettings.resolution.x, RenderSettings.resolution.y, RenderSettings.adapter_index)
		local old_display_mode = FullscreenWindowed._settings.display_mode
		FullscreenWindowed._settings.display_mode = choice
		FullscreenWindowed:save_settings()
		managers.menu:show_accept_gfx_settings_dialog(function ()
			managers.viewport:set_fullscreen(old_display_mode == 0)
			FullscreenWindowed.library.change_display_mode(old_display_mode, RenderSettings.resolution.x, RenderSettings.resolution.y, RenderSettings.adapter_index)
			FullscreenWindowed._settings.display_mode = old_display_mode
			FullscreenWindowed:save_settings()
			dm_item:set_value(FullscreenWindowed._settings.old_display_mode)
			if br_item then
				br_item:set_enabled(old_display_mode == 0)
			end
			self:refresh_node()
		end)
		if br_item then
			br_item:set_enabled(choice == 0)
		end
		self:refresh_node()
	end

	local fs_item = node:item("toggle_fullscreen")
	if not fs_item then
		local dm_item = node:item("multi_display_mode")
		if dm_item then
			dm_item:set_value(FullscreenWindowed._settings.display_mode)
		end
		return
	end

	node:delete_item("toggle_fullscreen")
	local params = {
		name = "multi_display_mode",
		text_id = "menu_display_mode",
		help_id = "menu_fullscreen_help",
		callback = "on_change_display_mode",
		filter = true
	}
	local data_node = {
		{
			value = 0,
			text_id = "menu_fullscreen",
			_meta = "option"
		},
		{
			value = 1,
			text_id = "menu_windowed",
			_meta = "option"
		},
		{
			value = 2,
			text_id = "menu_fullscreen_windowed",
			_meta = "option"
		},
		type = "MenuItemMultiChoice"
	}
	local dm_item = node:create_item(data_node, params)
	dm_item:set_value(FullscreenWindowed._settings.display_mode)
	node:insert_item(dm_item, 3)
end)

function MenuCallbackHandler:change_resolution(item)
	local old_resolution = RenderSettings.resolution

	if item:parameters().resolution == old_resolution then
		return
	end

	managers.viewport:set_resolution(item:parameters().resolution)
	managers.viewport:set_aspect_ratio(item:parameters().resolution.x / item:parameters().resolution.y)
	FullscreenWindowed.library.change_display_mode(FullscreenWindowed._settings.display_mode, item:parameters().resolution.x, item:parameters().resolution.y, RenderSettings.adapter_index)

	local function on_decline()
		managers.viewport:set_resolution(old_resolution)
		managers.viewport:set_aspect_ratio(old_resolution.x / old_resolution.y)
		FullscreenWindowed.library.change_display_mode(FullscreenWindowed._settings.display_mode, old_resolution.x, old_resolution.y, RenderSettings.adapter_index)
	end

	managers.menu:show_accept_gfx_settings_dialog(on_decline)
end

Hooks:Add("LocalizationManagerPostInit", "FullscreenWindowedAddLocalization", function(loc)
	local languages = {
		[Idstring("english"):key()] = "english",
		[Idstring("french"):key()] = "french",
		[Idstring("russian"):key()] = "russian",
		[Idstring("dutch"):key()] = "dutch",
		[Idstring("german"):key()] = "german",
		[Idstring("italian"):key()] = "italian",
		[Idstring("spanish"):key()] = "spanish",
		[Idstring("japanese"):key()] = "japanese",
		[Idstring("schinese"):key()] = "schinese",
		[Idstring("tchinese"):key()] = "tchinese",
		[Idstring("korean"):key()] = "korean",
		[Idstring("finnish"):key()] = "finnish",
		[Idstring("swedish"):key()] = "swedish",
		[Idstring("portuguese"):key()] = "portuguese",
		[Idstring("turkish"):key()] = "turkish",
		[Idstring("danish"):key()] = "danish",
		[Idstring("norwegian"):key()] = "norwegian",
		[Idstring("polish"):key()] = "polish"
	}

	local lang = languages[SystemInfo:language():key()]
	if lang == nil then
		lang = "english"
	end

	local lang_exist = io.file_is_readable(FullscreenWindowed.mod_path .. "/loc/" .. lang .. ".json")
	if not lang_exist then
		loc:add_localized_strings({
			["menu_display_mode"] = "Display Mode",
			["menu_windowed"] = "Windowed",
			["menu_fullscreen_windowed"] = "Fullscreen Windowed"
		})
		return
	end
	LocalizationManager:load_localization_file(FullscreenWindowed.mod_path .. "/loc/" .. lang .. ".json")
end)

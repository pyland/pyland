#include "python_embed_headers.hpp"

#include <string>
#include <boost/python.hpp>
#include <iostream>
#include "entity.hpp"
#include "game_engine.hpp"

namespace py = boost::python;

///
/// This is the shared object file that is generated to create the hooks
/// from the python code into the C++ objects.
/// If you want to add new methods to either Enity or GameEngine that you
/// want accessible to the python code, they have to be defined here.
///
BOOST_PYTHON_MODULE(wrapper_functions) {
    py::class_<Entity, boost::noncopyable>("Entity", py::no_init)
        .def_readwrite("id",      &Entity::id)
        .def_readwrite("name",    &Entity::name)
        .def("__set_game_speed",  &Entity::__set_game_speed)
        .def("get_instructions",  &Entity::get_instructions)
        .def("move_by",           &Entity::move_by)
        .def("move_east",         &Entity::move_east)
        .def("move_west",         &Entity::move_west)
        .def("move_north",        &Entity::move_north)
        .def("move_south",        &Entity::move_south)
        .def("is_moving",         &Entity::is_moving)
        .def("print_debug",       &Entity::py_print_debug)
        .def("read_message",      &Entity::read_message)
        .def("update_status",     &Entity::py_update_status)
        .def("set_solidity",      &Entity::set_solidity)
        .def("set_visible",       &Entity::set_visible)
        .def("is_solid",          &Entity::is_solid)
        .def("callback_test",     &Entity::callback_test)
        .def("wait",              &Entity::wait)
        .def("get_name",          &Entity::get_name)
        .def("get_location",      &Entity::get_location)
        .def("focus",             &Entity::focus)
        .def("is_focus",          &Entity::is_focus)
        .def("get_sprite",        &Entity::get_sprite)
        .def("set_sprite",        &Entity::set_sprite)
        .def("start_animating",   &Entity::start_animating)
        .def("pause_animating",   &Entity::pause_animating)
        .def("get_number_of_animation_frames",&Entity::get_number_of_animation_frames)
        .def("set_animation_frame",&Entity::set_animation_frame)
        .def("get_id",            &Entity::get_id)
        .def("get_position",      &Entity::get_position);

    py::class_<GameEngine, boost::noncopyable>("GameEngine", py::no_init)
        .def("create_object",     &GameEngine::create_object)
        .def("add_button",        &GameEngine::add_button)
        .def("set_cur_player",    &GameEngine::set_cur_player)
        .def("update_player_name",&GameEngine::update_player_name)
        .def("get_level_location",&GameEngine::get_level_location)
        .def("print_debug",       &GameEngine::print_debug)
        .def("show_dialogue",     &GameEngine::show_dialogue)
        .def("show_external_script_help",     &GameEngine::show_external_script_help)
        .def("close_external_script_help",     &GameEngine::close_external_script_help)
        .def("show_dialogue_with_options",     &GameEngine::show_dialogue_with_options)
        .def("get_config",        &GameEngine::get_config)
        .def("change_map",        &GameEngine::change_map)
        .def("get_tile_type",     &GameEngine::get_tile_type)
        .def("play_music",        &GameEngine::play_music)
        .def("register_input_callback",  &GameEngine::register_input_callback)
        .def("flush_input_callback_list",  &GameEngine::flush_input_callback_list)
        .def("show_py_scripter",  &GameEngine::show_py_scripter)
        .def("hide_py_scripter",  &GameEngine::hide_py_scripter)
        .def("enable_py_scripter",&GameEngine::enable_py_scripter)
        .def("disable_py_scripter",      &GameEngine::disable_py_scripter)
        .def("enable_script_editing",    &GameEngine::enable_script_editing)
        .def("disable_script_editing",   &GameEngine::disable_script_editing)
        .def("set_py_tabs",      &GameEngine::set_py_tabs)
        .def("show_external_script",      &GameEngine::show_external_script)
        .def("update_world",      &GameEngine::update_world)
        .def("update_level",      &GameEngine::update_level)
        .def("clear_level_text",      &GameEngine::clear_level_text)
        .def("update_coins",      &GameEngine::update_coins)
        .def("update_totems",     &GameEngine::update_totems)
        .def("clear_totems_text",     &GameEngine::clear_totems_text)
        .def("insert_to_scripter",&GameEngine::insert_to_scripter)
        .def("clear_scripter",    &GameEngine::clear_scripter)
        .def("get_script",        &GameEngine::get_script)
        .def("get_external_script", &GameEngine::get_external_script)
        .def("print_terminal",    &GameEngine::print_terminal)
        .def("get_terminal_text", &GameEngine::get_terminal_text)
        .def("get_objects_at",    &GameEngine::get_objects_at)
        .def("refresh_config",    &GameEngine::refresh_config)
        .def("set_ui_colours",    &GameEngine::set_ui_colours)
        .def("set_running",       &GameEngine::set_running)
        .def("set_finished",      &GameEngine::set_finished)
        .def("trigger_run",       &GameEngine::trigger_run)
        .def("get_run_script",    &GameEngine::get_run_script)
        .def("is_solid",          &GameEngine::is_solid)
        .def("add_event",         &GameEngine::add_event)
        .def("set_player_name",   &GameEngine::set_player_name)
        .def("get_player_name",   &GameEngine::get_player_name)
        .def("restart_level",     &GameEngine::restart_level)
        .add_static_property("INPUT_UP",  &GameEngine::INPUT_UP) //Add constants for different inputs.
        .add_static_property("INPUT_RIGHT",   &GameEngine::INPUT_RIGHT)
        .add_static_property("INPUT_DOWN",    &GameEngine::INPUT_DOWN)
        .add_static_property("INPUT_LEFT",    &GameEngine::INPUT_LEFT)
        .add_static_property("INPUT_RUN",     &GameEngine::INPUT_RUN)
        .add_static_property("INPUT_HALT",    &GameEngine::INPUT_HALT)
        .add_static_property("INPUT_ACTION",    &GameEngine::INPUT_ACTION)
        .add_static_property("TILE_TYPE_STANDARD",   &GameEngine::TILE_TYPE_STANDARD)
        .add_static_property("TILE_TYPE_SOLID",   &GameEngine::TILE_TYPE_SOLID)
        .add_static_property("TILE_TYPE_WATER",   &GameEngine::TILE_TYPE_WATER)
        .add_static_property("TILE_TYPE_QUICKSAND",   &GameEngine::TILE_TYPE_QUICKSAND);
}

#include "splashkit.h"
#include <vector>
using namespace std;

//                                      ●▬▬▬▬   »»»       𝗽𝗹𝗮𝘆𝗲𝗿.𝗵       «««  ▬▬▬▬▬●

#define MAX_VEL 3
#define MAX_SPAWN 1000
#define MIN_SPAWN -1000
#define MAX_SPAWN_RANGE 2000

/**
 * enum for the different parts of player 
 * the main ship and the force field
 */
enum player_upgrade
{
    SHIELDED,
    PLAYER
};

/**
 * The player data keeps track of all of the information related to the player.
 * 
 * @field   player_sprite   The player's sprite - used to track position and movement
 * @field   score           The current score for the player
 * @field   upgrade         current upgrade on player ship
 * @field   fuel_pct        the prcentage of fuel in player ship
 * @field   shield          checks if player has shield or not
 * @field   game_over       checks if the player lost or not
 */
struct player_data
{
    sprite player_sprite;
    int score;
    player_upgrade upgrade;
    double fuel_pct;
    bool shield;
    bool game_over;
};

/**
 * Creates a new player in the centre of the screen with the default ship.
 * 
 * @returns     The new player data
 */
player_data new_player();

/**
 * Draws the player to the screen. 
 * 
 * @param player_to_draw    The player to draw to the screen
 */
void draw_player(const player_data &player_to_draw);

/**
 * Actions a step update of the player - moving them and adjusting the camera.
 * 
 * @param player_to_update      The player being updated
 */
void update_player(player_data &player_to_update);

/**
 * Read user input and update the player based on this interaction.
 * 
 * @param player    The player to update
 */
void handle_input(player_data &player);

//                                      ●▬▬▬▬   »»»       𝗽𝗹𝗮𝘆𝗲𝗿.cpp       «««  ▬▬▬▬▬●

player_data new_player()
{
    player_data result;
    result.player_sprite = create_sprite(bitmap_named("player"));

    /**
     * @brief this code is used to add the force field bitmap as 
     * a layer onto the player and to hide it till he picks
     * up the shield power up
     * 
     */
    vector_2d force_loc; // offsets the bitmap of force field so it fits perfectly on the player
    force_loc.x = -25;
    force_loc.y = -35;

    sprite_add_layer(result.player_sprite, bitmap_named("force_field"), "force_field");
    sprite_set_layer_offset(result.player_sprite, 1, force_loc);
    sprite_hide_layer(result.player_sprite, 1);

    // Position in the centre of the initial screen
    sprite_set_x(result.player_sprite, (screen_width() - sprite_width(result.player_sprite)) / 2);
    sprite_set_y(result.player_sprite, (screen_height() - sprite_height(result.player_sprite)) / 2);

    return result;
}

void update_camera_position(double player_x, double player_y)
{
    // Test edge of screen boundaries to adjust the camera
    // it is made so the edges are just the centre of the screen
    // so the player remains in centre and world moves around him
    double left_edge = camera_x() + screen_width() / 2;
    double right_edge = left_edge + screen_width() - 2 * screen_width() / 2;
    double top_edge = camera_y() + screen_height() / 2;
    double bottom_edge = top_edge + screen_height() - 2 * screen_height() / 2;

    // Test if the player is outside the area and move the camera
    // the player will appear to stay still and everything else
    // will appear to move :)

    // Test top/bottom of screen
    if (player_y < top_edge)
    {
        move_camera_by(0, player_y - top_edge);
    }
    else if (player_y > bottom_edge)
    {
        move_camera_by(0, player_y - bottom_edge);
    }

    // Test left/right of screen
    if (player_x < left_edge)
    {
        move_camera_by(player_x - left_edge, 0);
    }
    else if (player_x > right_edge)
    {
        move_camera_by(player_x - right_edge, 0);
    }
}

void draw_player(const player_data &player_to_draw)
{
    draw_sprite(player_to_draw.player_sprite);
}

void update_player(player_data &player_to_update)
{
    point_2d sprite_center = center_point(player_to_update.player_sprite);

    update_sprite(player_to_update.player_sprite);

    update_camera_position(sprite_center.x, sprite_center.y);
}

void handle_input(player_data &player)
{
    point_2d loc_play, loc_mouse, pos;

    if (mouse_down(LEFT_BUTTON)) // code executed only if LMB is down
    {
        loc_play = center_point(player.player_sprite); // position of player's center
        loc_mouse = mouse_position();                  // position of mouse

        // makes the mouse position relative to player rather than the screen
        pos.x = loc_play.x + loc_mouse.x - screen_width() / 2;
        pos.y = loc_play.y + loc_mouse.y - screen_height() / 2;

        // makes a vector out of the player and relative mouse position
        // and also caps it with a limit.
        vector_2d vel = vector_limit(vector_point_to_point(loc_play, pos), MAX_VEL);
        sprite_set_velocity(player.player_sprite, vel);
    }
    else if (mouse_clicked(RIGHT_BUTTON)) // code executed only if RMB is clicked
    {
        // stops the player wherever it is
        // by making velocity value 0
        sprite_set_dx(player.player_sprite, 0);
        sprite_set_dy(player.player_sprite, 0);
    }
}

//                                      ●▬▬▬▬   »»»       entity.𝗵       «««  ▬▬▬▬▬●

/**
 * Different options for the spawning entities.
 * Adjusts the image used.
 */
enum entity_type
{
    SHIELD,
    STAR,
    FUEL,
    ALLY_1,
    ALLY_2,
    ALLY_3,
    ALLY_4,
    FOE
};

/**
 * The player data keeps track of all of the information related to the spawn entities.
 * 
 * @field   entity_sprite     The entity sprite
 * @field   type             the type of entity
 */
struct entity_data
{
    entity_type type;
    sprite entity_sprite;
};

/**
 * Creates a new entity at a random location on the screen.
 * 
 * @param x a random point in x axes
 * @param y a random point in y axes
 */
entity_data entity_spawn(double x, double y);

/**
 * Draws the entity to the screen. 
 * 
 * @param entity_draw    The entity to draw to the screen
 */
void draw_entity(const entity_data &entity_draw);

/**
 * Actions an update of the entity - 
 * moving the entity in a random direction in a random speed.
 * 
 * @param entity_update      The entity being updated
 */
void update_entity(entity_data &entity_update);

//                                      ●▬▬▬▬   »»»       entity.cpp       «««  ▬▬▬▬▬●

/**
 * The entity_bitmap function converts a entity type into a 
 * bitmap that can be used.
 * 
 * @param type  The type of entity
 * @return      The bitmap matching this entity type
 */
bitmap entity_bitmap(entity_type type)
{
    switch (type)
    {
    case SHIELD:
        return bitmap_named("shield");
    case STAR:
        return bitmap_named("star");
    case ALLY_1:
        return bitmap_named("ally_1");
    case ALLY_2:
        return bitmap_named("ally_2");
    case ALLY_3:
        return bitmap_named("ally_3");
    case ALLY_4:
        return bitmap_named("ally_4");
    case FOE:
        return bitmap_named("foe");
    default:
        return bitmap_named("fuel");
    }
}

/**
 * The spawn_chance use rnd() function to choose 
 * the type of entity to spawn 
 * 
 * @return      the type of entity choosen through percentage chance
 */
entity_type spawn_chance()
{
    // 30 percent chance that the spawned entity will be an ally
    if (rnd() <= 0.3)
        return static_cast<entity_type>(rnd(3, 7));

    // 21 percent chance(out of 100) that the spawned entity is a foe
    else if (rnd() <= 0.3)
        return FOE;

    // the rest chance is that spawned entity is a power up
    else
        return static_cast<entity_type>(rnd(3));
}

entity_data entity_spawn(double x, double y)
{
    entity_data result;
    entity_type type = spawn_chance();
    result.type = type;
    result.entity_sprite = create_sprite(entity_bitmap(type));

    // sets a random location of the entity on the screen
    sprite_set_x(result.entity_sprite, x);
    sprite_set_y(result.entity_sprite, y);

    /**
     * @brief 'if' statement checks if the spawn entity is not an ally
     * 
     * I have made my game such that the allies don't move and stay 
     * at the location they spawn, so this 'if' block helps me with that.
     * 
     * entities other than allies go through the 'if' block
     * and get random velocities.
     */
    if (type != ALLY_1 && type != ALLY_2 && type != ALLY_3 && type != ALLY_4)
    {
        // sets a random speed of the entity
        sprite_set_dx(result.entity_sprite, rnd() * 4 - 2);
        sprite_set_dy(result.entity_sprite, rnd() * 4 - 2);
    }
    return result;
}

void draw_entity(const entity_data &entity_draw)
{
    draw_sprite(entity_draw.entity_sprite);
}

void update_entity(entity_data &result)
{
    update_sprite(result.entity_sprite);
}

//                                      ●▬▬▬▬   »»»       space_wars.𝗵       «««  ▬▬▬▬▬●

/**
 * The game_data keeps track of all of the information related to the game.
 * 
 * @field   player          player created for the game
 * @field   spawn           the vector which will contain the entities
 * @field   game_over_by    checks if player lost by getting hit or due to low fuel 
 */
struct game_data
{
    player_data player;
    vector<entity_data> spawner;
    int game_over_by;
};

/**
 * Creates a new game with a new player on the screen.
 */
game_data new_game();

/**
 * Draws the game on the screen. 
 * 
 * @param game_draw    The game to draw to the screen
 */
void draw_game(game_data &game_draw);

/**
 * updates the game checking for changes in player
 * also checks the updates and spawning of power ups 
 * 
 * @param game_update      The game being updated
 */
void update_game(game_data &game_update);

//                                      ●▬▬▬▬   »»»       space_wars.cpp       «««  ▬▬▬▬▬●

/**
 * @brief this function is a special one
 * it is used to give random sound to the allies
 * when they are rescued
 * 
 * till now there are 3 things the ally can say
 * - 'thank you' in japanese
 * - 'thank you' in spanish
 * - 'thank you' in hindi
 * ps: offcourse in their alien sounds
 * 
 * this function can be used to add more sounds in the future.
 */
void random_noise()
{
    int num = rnd(3);
    switch (num)
    {
    case 0:
        play_sound_effect("thanks1");
        break;
    case 1:
        play_sound_effect("thanks2");
        break;
    default:
        play_sound_effect("thanks3");
        break;
    }
}

/**
 * this function is used to apply a specific power up
 * or see if player picked up ally
 * or if player decided to attack the foe without thinking
 * 
 * @param game  the main game variable used in various tasks
 * @param i     an integer used to know the index of power up in vector
 */
void apply_spawn(game_data &game, int idx)
{
    // Increasing the value only if the percentage is less than 100
    if (game.spawner[idx].type == FUEL)
    {
        play_sound_effect("fuel");

        // fills in 25% of total capacity of fuel tank
        if (game.player.fuel_pct < 0.75)
            game.player.fuel_pct += 0.25;
        else
            game.player.fuel_pct += 1 - game.player.fuel_pct;
    }
    else if (game.spawner[idx].type == STAR)
    {
        play_sound_effect("star");
        game.player.score += 30;
    }
    else if (game.spawner[idx].type == ALLY_1 or game.spawner[idx].type == ALLY_2)
    {
        random_noise();
        game.player.score += 10;
    }
    else if (game.spawner[idx].type == ALLY_3 or game.spawner[idx].type == ALLY_4)
    {
        random_noise();
        game.player.score += 10;
    }
    else if (game.spawner[idx].type == FOE)
    {
        if (game.player.shield == false)
        {
            game.player.game_over = true;
            game.game_over_by = 1;

            play_sound_effect("hit");
        }
        else
        {
            sprite_hide_layer(game.player.player_sprite, 1);
            play_sound_effect("shield_hit");
        }
        game.player.shield = false;
    }
    else
    {
        sprite_show_layer(game.player.player_sprite, 1);
        play_sound_effect("activated");

        game.player.shield = true;
    }
}

/**
 * this function is used to remove a specific power up
 * 
 * @param game  the main game variable used in various tasks
 * @param i     an integer used to know the index of power up in vector
 */
void remove_spawn(game_data &game, int idx)
{
    int last;

    if (idx >= 0 && idx < game.spawner.size())
    {
        last = game.spawner.size() - 1;
        game.spawner[idx] = game.spawner[last];
        game.spawner.pop_back();
    }
}

/**
 * this function is used to remove (despawn) power up
 * if it goes out of 2000 pixel spawner radius of player.
 * 
 * @param game  the main game variable used in various tasks
 */
void check_entity_position(game_data &game)
{
    point_2d location = center_point(game.player.player_sprite);

    int x, y;
    x = (int)location.x;
    y = (int)location.y;

    for (int i = 0; i < game.spawner.size(); i++)
    {
        double entity_x = sprite_x(game.spawner[i].entity_sprite);
        double entity_y = sprite_y(game.spawner[i].entity_sprite);

        // The entity is removed if it goes out of the 2000 pixels from player
        if (entity_x > x + MAX_SPAWN or entity_y > y + MAX_SPAWN or entity_x < x + MIN_SPAWN or entity_y < y + MIN_SPAWN)
        {
            remove_spawn(game, i);
        }
    }
}

/**
 * this function is used to check collision of power up with player
 * 
 * @param game  the main game variable used in various tasks
 */
void check_collision(game_data &game)
{
    for (int num = game.spawner.size() - 1; num >= 0; num--)
    {
        if (sprite_collision(game.player.player_sprite, game.spawner[num].entity_sprite))
        {
            apply_spawn(game, num);
            remove_spawn(game, num);
        }
    }
}

/**
 * The entity_bitmap function converts a power up type into a 
 * bitmap that can be used.
 * 
 * @param game  the main game variable used in various tasks
 */
void spawn_entity(game_data &game)
{
    point_2d location = center_point(game.player.player_sprite);

    int x, y;

    x = (int)location.x;
    y = (int)location.y;

    // spawns entities in spawning area of the player
    game.spawner.push_back(entity_spawn(x + rnd(MIN_SPAWN, MAX_SPAWN), y + rnd(MIN_SPAWN, MAX_SPAWN)));
}

game_data new_game()
{
    game_data new_game;

    new_game.player = new_player();
    new_game.player.shield = false;
    new_game.player.game_over = false;
    new_game.player.fuel_pct = 1;
    new_game.player.score = 0;
    new_game.game_over_by = 1;

    return new_game;
}

void draw_game(game_data &game_draw)
{
    draw_player(game_draw.player);

    for (int num = 0; num < game_draw.spawner.size(); num++)
    {
        draw_entity(game_draw.spawner[num]);
    }
}

void update_game(game_data &game_update)
{
    // limits the total entities on map to be 20
    if (rnd() < 0.02 && game_update.spawner.size() < 20)
        spawn_entity(game_update);

    update_player(game_update.player);

    check_collision(game_update);
    check_entity_position(game_update);

    for (int num = 0; num < game_update.spawner.size(); num++)
    {
        update_entity(game_update.spawner[num]);
    }
}

//                                      ●▬▬▬▬   »»»       program.cpp       «««  ▬▬▬▬▬●

/**
 * Load the game images, sounds, etc.
 * also text for hud.
 */
void load_resources()
{
    load_resource_bundle("game_bundle", "space_wars.txt");
}

/**
 * this is used to simplify the location to integer values
 * for easier display on screen. 
 * 
 * @param player    the player whose location is checked
 * @return          string with location of player for hud
 */
string loc_to_string(const player_data &player)
{
    point_2d location = center_point(player.player_sprite);
    int x, y;
    x = (int)location.x;
    y = (int)location.y;
    return to_string(x) + " " + to_string(y);
}

/**
 * this function is used to get the position of a specific entity
 * relative to the size of the screen dimensions.
 * 
 * @param game  the main game variable used in various tasks
 * @param i     an integer used to know the index of entity in vector
 * @return      the co-ordinates of certain entity relative to your screen size 
 */
point_2d spawn_mini_map_coordinate(const game_data &game, int i)
{
    point_2d location = center_point(game.player.player_sprite);
    int x, y;
    x = (int)location.x;
    y = (int)location.y;
    double entity_x, entity_y, mini_map_x, mini_map_y;

    entity_x = sprite_x(game.spawner[i].entity_sprite);
    entity_y = sprite_y(game.spawner[i].entity_sprite);

    // Calculating the coordinate of the entity according to the minimap
    mini_map_x = (entity_x - x - MIN_SPAWN) / MAX_SPAWN_RANGE * 100 + 20;
    mini_map_y = (entity_y - y - MIN_SPAWN) / MAX_SPAWN_RANGE * 100 + 20;

    return point_at(mini_map_x, mini_map_y);
}

/**
 * this procedure is used to draw a mini map in the game
 * it also draws pixels for position of several entities and the player
 * 
 * @param game  The game details used for various tasks
 */
void draw_minimap(const game_data &game)
{
    fill_rectangle(COLOR_BLACK, 20, 20, 100, 100, option_to_screen());
    draw_rectangle(COLOR_WHITE, 20, 20, 100, 100, option_to_screen());

    for (int i = 0; i < game.spawner.size(); i++)
    {
        if (game.spawner[i].type == FOE)
            // Drawing position of the foe with red colour
            draw_pixel(rgba_color(255, 0, 0, 240), spawn_mini_map_coordinate(game, i), option_to_screen());

        else if (game.spawner[i].type == SHIELD or game.spawner[i].type == STAR or game.spawner[i].type == FUEL)
            // Drawing position of the power ups with green colour
            draw_pixel(rgba_color(0, 255, 0, 240), spawn_mini_map_coordinate(game, i), option_to_screen());

        else
            // Drawing position of the allies with cyan colour
            draw_pixel(rgba_color(0, 255, 255, 240), spawn_mini_map_coordinate(game, i), option_to_screen());
    }

    //Drawing position of the player with white colour
    draw_pixel(rgba_color(255, 255, 255, 255), point_at(70, 70), option_to_screen());
}

/**
 * procedure to draw the force field sign
 * on the hud to indicate that shield is active
 * it depens on :-
 * shield in struct player_data
 */
void shield_disp(const player_data &player_shield_display)
{
    if (player_shield_display.shield)
    {
        draw_bitmap("force", 65, 510, option_to_screen());
    }
}

/**
 * this procedure to display fuel in the hud
 * the amount of fuel displayed depends on :-
 *  
 * fuel_pct in struct player_data
 */
void fuel_checker(game_data &game_player_fuel)
{
    double part_width = game_player_fuel.player.fuel_pct * bitmap_width("full");

    draw_text("FUEL: ", COLOR_BRIGHT_GREEN, "font", 25, 10, 555, option_to_screen());
    draw_bitmap("empty", 70, 550, option_to_screen());
    draw_bitmap("full", 70, 550, option_part_bmp(0, 0, part_width, bitmap_height("full"), option_to_screen()));

    if (sprite_dx(game_player_fuel.player.player_sprite) != 0)
        game_player_fuel.player.fuel_pct -= 0.00028;

    if (game_player_fuel.player.fuel_pct <= 0)
    {
        game_player_fuel.game_over_by = 2;
        game_player_fuel.player.game_over = true;
    }
}

/**
 * procedure to display the hud
 * and all the features in it.
 */
void display_hub(game_data &game)
{
    // score of the player
    draw_text("SCORE: " + to_string(game.player.score), COLOR_WHITE, 1090, 590, option_to_screen());

    // location of the player
    draw_text("LOCATION: " + loc_to_string(game.player), COLOR_WHITE, screen_width() / 2 - 60, screen_height() - 15, option_to_screen());

    // background of hud display
    draw_bitmap("hud", 1, 490, option_to_screen());

    fuel_checker(game);
    shield_disp(game.player);
    // star_disp(game.player);

    draw_minimap(game);
}

/**
 * @brief   procedure to display the welcome screen
 *          with the rules and controls of game
 * 
 * @param choice    it decides what to display as per users choice
 */
void welcome_screen(int &choice)
{
    choice = 1;

    while (not quit_requested())
    {
        process_events();
        clear_screen(COLOR_BLACK);

        if (key_typed(BACKSPACE_KEY) or choice == 1)
        {
            choice = 1;
            draw_bitmap("1", 0, 0, option_to_screen());
        }

        if (key_typed(NUM_1_KEY) or choice == 2)
        {
            choice = 2;
            draw_bitmap("2", 0, 0, option_to_screen());
        }

        if (key_typed(SPACE_KEY))
            break;

        refresh_screen(60);
    }
}

/**
 * @brief   procedure to display the end screen
 *          with the score of player
 * 
 * @param game      the main game variable used in various tasks
 * @param choice    displays end screen as per players loss
 */
void end_screen(const game_data &game, int &choice)
{
    choice = game.game_over_by;

    while (not quit_requested())
    {
        process_events();
        clear_screen(COLOR_BLACK);

        if (choice == 2)
        {
            draw_bitmap("end_fuel", 0, 0, option_to_screen());
        }

        else
        {
            draw_bitmap("end_hit", 0, 0, option_to_screen());
        }

        if (key_typed(SPACE_KEY))
        {
            choice = 1;
            break;
        }

        if (key_typed(X_KEY))
        {
            choice = 0;
            break;
        }
        draw_text("SCORE - " + to_string(game.player.score), COLOR_WHITE, "game_font", 35, 390, 350, option_to_screen());
        refresh_screen(60);
    }
}

/**
 * Entry point.
 * 
 * Manages the initialisation of data, the event loop, and quitting.
 */
int main()
{
    open_window("space wars", 1200, 600);
    load_resources();

    int choice = 1;
    while (not quit_requested())
    {

        game_data game = new_game();

        welcome_screen(choice);

        while (not quit_requested())
        {
            // checks and plays music if not playing
            if (not music_playing())
                play_music("bg");

            // Handle input to adjust player movement
            process_events();
            handle_input(game.player); // Perform movement and update the camera

            update_game(game);

            // Redraw everything
            clear_screen(COLOR_BLACK);

            // draw game and hud
            draw_game(game);
            display_hub(game);

            if (game.player.game_over)
                break;

            refresh_screen(60);
        }

        stop_music();

        end_screen(game, choice);
        if (choice == 0)
            break;
    }
    return 0;
}

/*.-----------.| thank you |'-----------'*/
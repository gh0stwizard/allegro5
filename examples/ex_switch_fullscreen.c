/*
 * Demonstrate switching between both windowed & full screen modes.
 * Hotkey: F11
 */

#include "allegro5/allegro.h"

#include "common.c"

#define ENABLE_VSYNC 1

static void
switch_mode(ALLEGRO_DISPLAY **display);

static void
log_key(char const *how, int keycode, int unichar, int modifiers);


extern int
main(int argc, char **argv)
{
	ALLEGRO_DISPLAY *display;
	ALLEGRO_EVENT_QUEUE *queue;
	ALLEGRO_TIMER *timer;

	if (!al_init()) {
		abort_example("Could not init Allegro.\n");
	}

	open_log_monospace();

	al_install_keyboard();

	al_set_new_display_flags(ALLEGRO_WINDOWED);
	display = al_create_display(640, 480);
	if (!display) {
		abort_example("Failed to create display.\n");
	}

	timer = al_create_timer(0.5);

	queue = al_create_event_queue();
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));
	al_register_event_source(queue, al_get_timer_event_source(timer));

	al_start_timer(timer);

	while (1) {
		ALLEGRO_EVENT event;
		al_wait_for_event(queue, &event);

		switch (event.type) {
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			goto done;

		case ALLEGRO_EVENT_DISPLAY_LOST:
			log_printf("DISPLAY LOST\n");
			al_clear_keyboard_state(display);
			break;

		case ALLEGRO_EVENT_DISPLAY_FOUND:
			log_printf("DISPLAY FOUND\n");
			break;

		case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
			log_printf("SWITCH IN\n");
			break;

		case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
			log_printf("SWITCH OUT\n");
			al_clear_keyboard_state(display);
			break;

		case ALLEGRO_EVENT_DISPLAY_CONNECTED:
			log_printf("DISPLAY CONNECTED\n");
			break;

		case ALLEGRO_EVENT_DISPLAY_DISCONNECTED:
			log_printf("DISPLAY DISCONNECTED\n");
			al_clear_keyboard_state(display);
			break;

		case ALLEGRO_EVENT_KEY_DOWN:
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
				goto done;

			log_key("KEY_DOWN", event.keyboard.keycode, 0, 0);

			/* re-create display */
			if (event.keyboard.keycode == ALLEGRO_KEY_F11) {
				al_unregister_event_source(queue, al_get_keyboard_event_source());
				al_unregister_event_source(queue, al_get_display_event_source(display));
				switch_mode(&display);
				al_register_event_source(queue, al_get_display_event_source(display));
				al_register_event_source(queue, al_get_keyboard_event_source());
				al_clear_keyboard_state(display);
			}
			break;

		case ALLEGRO_EVENT_KEY_UP:
			log_key("KEY_UP", event.keyboard.keycode, 0, 0);
			break;

		case ALLEGRO_EVENT_KEY_CHAR: {
			char const *label = (event.keyboard.repeat ? "repeat" : "KEY_CHAR");
			log_key(label,
				event.keyboard.keycode,
				event.keyboard.unichar,
				event.keyboard.modifiers);
			break;
		}

		case ALLEGRO_EVENT_TIMER:
			al_set_target_backbuffer(display);
			al_clear_to_color(al_map_rgb(rand() % 255, rand() % 255, rand() % 255));
			al_flip_display();
			break;
		}
	}

done:
	close_log(false);
	al_destroy_event_queue(queue);
	al_destroy_display(display);
	al_destroy_timer(timer);

	return 0;
}


static void
switch_mode(ALLEGRO_DISPLAY **display)
{
	ALLEGRO_DISPLAY *d = *display;
	int flags = al_get_new_display_flags();
	int w, h;

	al_reset_new_display_options();

	if (flags & ALLEGRO_FULLSCREEN) {
		w = 640;
		h = 480;
		al_set_new_display_flags(ALLEGRO_WINDOWED);
	}
	else {
		ALLEGRO_MONITOR_INFO monitor;
		al_get_monitor_info(0, &monitor);
		w = monitor.x2 - monitor.x1;
		h = monitor.y2 - monitor.y1;
		al_set_new_display_flags(ALLEGRO_FULLSCREEN);
		/* 0: default, 1: force enable, 2: force disable */
		if (ENABLE_VSYNC) {
			al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_REQUIRE);
		}
	}

	al_destroy_display(d);
	d = al_create_display(w, h);

	if (!d)
		abort_example("Failed to re-create display.\n");

	*display = d;
}


static void
log_key(char const *how, int keycode, int unichar, int modifiers)
{
	char multibyte[5] = { 0, 0, 0, 0, 0 };
	const char* key_name;

	al_utf8_encode(multibyte, unichar <= 32 ? ' ' : unichar);
	key_name = al_keycode_to_name(keycode);
	log_printf("%-8s  code=%03d, char='%s' (%4d), modifiers=%08x, [%s]\n",
		how, keycode, multibyte, unichar, modifiers, key_name);
}


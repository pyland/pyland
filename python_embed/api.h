struct Player {
	Player(int startx, int starty);
	void move(int dx, int dy);
	void speak_position();
	int x;
	int y;
};
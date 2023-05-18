class SoundManager
{
public:
	static void init();
	static void add(std::string path, std::string name);
	static void oneShot(std::string path);
	static void start(std::string name);
	static void stop(std::string name);
	static bool isPlaying(std::string name);
	static bool isEnd(std::string name);
	static void reset(std::string name);
	static void fade(std::string name, float begin, float end, float millis);
};
// 导入库
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <random>
#include <iostream>
#include <string>
#include <cmath>
// 声明空间
using namespace sf;

// 基础数据
unsigned int Winwidth = 1000;
unsigned int Winheight = 500;
float Winwidthf = 1000.f;
float Winheightf = 500.f;
unsigned int FPS = 60;
Color BLACK = Color(0, 0, 0, 255);
Color WHITE = Color(255, 255, 255, 255);
Color RED = Color(255, 0, 0, 255);
Color GREEN = Color(0, 255, 0, 255);
Color BLUE = Color(0, 0, 255, 255);
Color YELLOW = Color(255, 255, 0, 255);
Color GOLD = Color(255, 215, 0, 255);
Color ORANGE = Color(255, 165, 0, 255);
Color SKYBLUE = Color(135, 206, 235, 255);
Color TRANSLUCENT_BLACK = Color(0, 0, 127);
Color TRANSPARENT = Color(255, 255, 255, 0);
std::string bgm_path[15] = {"BGM/Position Music - Qiu Mansion.ogg",
						"BGM/Audiomachine - Breath and Life.ogg", 
						"BGM/C418 - stranger_think.ogg", 
						"BGM/Dark Winter Music - Novera.ogg", 
						"BGM/Groove Addicts - Seafights.ogg", 
						"BGM/Kyle - 克罗地亚狂想曲.ogg", 
						"BGM/ボイジャー,濱田龍臣 - GEEDの証 (TVサイズ).ogg", 
						"BGM/陈墨染 - SO TIRED.ogg", 
						"BGM/光宗信吉 - 熱き決闘者たち.ogg", 
						"BGM/可我只是海 - Wake OAO （完整版）（娘子 remix）.ogg", 
						"BGM/空耿耿 - Show it Ben.ogg", 
						"BGM/小澤正澄 - attraction.ogg", 
						"BGM/Bymyself-001.ogg", 
						"BGM/Position Music - The New York Nightfall.ogg", 
						"BGM/Position Music - Man At Arms.ogg"};
Font consola;
SoundBuffer es_music;
Sound es_sound;
SoundBuffer hurt_music;
Sound hurt_sound;
Music bgm;
Texture bg;
SoundSource::Status bgm_status = bgm.getStatus();

// 数据
// 创建窗口
RenderWindow window(VideoMode(Winwidth, Winheight), "Game");
// 玩家坐标
Vector2f player(200.f, 200.f);
// AI坐标
Vector2f ai(800.f, 200.f);
// 创建随机数引擎
std::random_device random;
std::mt19937 gen(random());
// 设置AI移动延迟
Clock ai_sports_delay;
Time ai_sports_delay_current = ai_sports_delay.getElapsedTime();
bool ai_can_move = true;
const int ai_sports_delay_length = 300;
// 设置AI跳跃部分的随机数范围
std::uniform_int_distribution<int> ai_jumping_range(1, 600);
int ai_jumping_num = ai_jumping_range(gen);
// 设置AI攻击部分的随机数范围
std::uniform_int_distribution<int> ai_attack_range(1, 1000);
int ai_attack_num = ai_attack_range(gen);
// 设置暴击随机数
std::uniform_int_distribution<int> boom(1, 100);
int boom_num = boom(gen);
// 设置音乐切换随机数
std::uniform_int_distribution<int> bgm_switch(0, 14);
int bgm_switch_num = bgm_switch(gen);
// 暴击倍率
float boom_times = 1.2f;
// x轴差值
float x_difference = std::fabs(player.x - ai.x);
// 移动速度
const float speed = 5.f;
float player_speed = speed;
float ai_speed = speed;
// 跳跃时间
const float jumping_time = 0.8f;
// 跳跃高度
const float jumping_height = 8.f;
float player_jumping_height = 0.f;
float ai_jumping_height = 0.f;
// 重力
const float G = jumping_height * 2 / (jumping_time * FPS);
// 空气阻力
const float AR = 0.05f;
// 玩家跳跃标志
bool player_jumping_bool = false;
bool ai_jumping_bool = false;
bool player_is_jumping = false;
bool ai_is_jumping = false;
// 玩家移动量
float player_movement = 0.f;
float ai_movement = 0.f;
// 移动量扣除1体力时的大小
float movement_max = 5000.f;
// 关卡
int level_max = 100;
int level = 1;
// 生命值
float player_life_max = 80.f + 20.f * level;
float ai_life_max = 50.f + 30.f * level;
float player_life = player_life_max;
float ai_life = ai_life_max;
float player_life_ratio = 200.f / player_life_max;
float ai_life_ratio = 200.f / ai_life_max;
// 体力
float player_strength_max = 5.f + 5.f * level;
float ai_strength_max = 8.f + 5.f * level;
float player_strength = player_strength_max;
float ai_strength = ai_strength_max;
float player_strength_ratio = 200.f / player_strength_max;
float ai_strength_ratio = 200.f / ai_strength_max;
// 攻击持续时间
int es_time = 600;  // 肘击elbow strike
// 设置攻击持续的时钟
Clock player_attack_clock;
Time player_attack_time = player_attack_clock.getElapsedTime();
Clock ai_attack_clock;
Time ai_attack_time = ai_attack_clock.getElapsedTime();
// 设置攻击标志  // 0:无  1:肘击
int player_is = 0;
int player_is_attack = 0;
int ai_is = 0;
int ai_is_attack = 0;
// 设置攻击消耗体力
int es_strength = 1;
// 设置攻击伤害
int es_life = 10;
// 设置击退
Vector2f es_go_away(50, 50);
// 胜利
bool winning = false;
// 失败
bool losing = false;



// 类
struct BasicShape {
	void line(RenderWindow& window, Color color, float length, Vector2f position, float thickness, float angle) {
		RectangleShape line(Vector2f(length, thickness));
		line.setFillColor(color);
		line.setPosition(position);
		line.setRotation(angle);
		window.draw(line);
	}
	void linep(RenderWindow& window, Color color, Vector2f start, Vector2f end, float thickness) {  // line_position以位置来画线
		Vector2f direction = end - start;
		float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
		float angle = std::atan2(direction.y, direction.x) * 180.f / 3.141592653589793f;
		RectangleShape line(Vector2f(length, thickness));
		line.setFillColor(color);
		line.setPosition(start);
		line.setRotation(angle);
		window.draw(line);
	}
	void rectangle(RenderWindow& window, Color fill_color, Color outline_color, Vector2f size, Vector2f position, float outline_thickness, float angle) {
		RectangleShape rect(size);
		rect.setFillColor(fill_color);
		rect.setOutlineColor(outline_color);
		rect.setOutlineThickness(outline_thickness);
		rect.setPosition(position);
		rect.setRotation(angle);
		window.draw(rect);
	}
	void circle(RenderWindow& window, Color fill_color, Color outline_color, float radius, Vector2f position, float outline_thickness) {
		CircleShape circle(radius);
		circle.setFillColor(fill_color);
		circle.setOutlineColor(outline_color);
		circle.setOutlineThickness(outline_thickness);
		circle.setPosition(position);
		window.draw(circle);
	}
	void words(RenderWindow& window, Font& font, Color fill_color, Color outline_color, String string, Vector2f position, int size, float outline_thickness, float angle) {
		Text text;
		text.setFont(font);
		text.setString(string);
		text.setCharacterSize(size);
		text.setFillColor(fill_color);
		text.setOutlineColor(outline_color);
		text.setOutlineThickness(outline_thickness);
		text.setPosition(position);
		text.setRotation(angle);
		window.draw(text);
	}
	void pictures(RenderWindow& window, Texture& texture, Vector2f size, Vector2f position) {
		Vector2u originalSize = texture.getSize();
		float ScaleX = size.x / originalSize.x;
		float ScaleY = size.y / originalSize.y;
		Sprite sprite;
		sprite.setTexture(texture);
		sprite.setScale(ScaleX, ScaleY);
		sprite.setPosition(position);
		window.draw(sprite);
	}
};
class Element {
private:
	BasicShape shape;
public:
	void floor() {
		shape.line(window, TRANSLUCENT_BLACK, 1000.f, Vector2f(0.f, 400.f), 1.f, 0.f);
	}
	void I_man(Color color, Vector2f position) {
		Vector2f rect_position(position.x - 10.f, position.y - 70.f);
		Vector2f circle_position(position.x - 10.f, position.y - 100.f);
		shape.rectangle(window, TRANSPARENT, color, Vector2f(20.f, 70.f), rect_position, 2.f, 0.f);
		shape.circle(window, TRANSPARENT, color, 10.f, circle_position, 2.f);
	}
	void situation_part() {
		shape.linep(window, TRANSLUCENT_BLACK, Vector2f(500.f, 400.f), Vector2f(500.f, 500.f), 1.f);  // 绘制分割线
	}
	void life_line() {
		shape.rectangle(window, TRANSPARENT, BLACK, Vector2f(200.f, 20.f), Vector2f(0.f, 420.f), 1.f, 0.f);
		shape.rectangle(window, TRANSPARENT, BLACK, Vector2f(200.f, 20.f), Vector2f(0.f, 460.f), 1.f, 0.f);
		shape.rectangle(window, TRANSPARENT, BLACK, Vector2f(200.f, 20.f), Vector2f(800.f, 420.f), 1.f, 0.f);
		shape.rectangle(window, TRANSPARENT, BLACK, Vector2f(200.f, 20.f), Vector2f(800.f, 460.f), 1.f, 0.f);
		Color pl;
		Color ail;
		if (player_life_max * 0.8 < player_life) pl = GREEN;
		else if (player_life_max * 0.5 < player_life and player_life <= player_life_max * 0.8) pl = YELLOW;
		else if (player_life_max * 0.2 < player_life and player_life <= player_life_max * 0.5) pl = ORANGE;
		else if (player_life <= player_life_max * 0.2) pl = RED;
		if (ai_life_max * 0.8 < ai_life) ail = GREEN;
		else if (ai_life_max * 0.5 < ai_life and ai_life <= ai_life_max * 0.8) ail = YELLOW;
		else if (ai_life_max * 0.2 < ai_life and ai_life <= ai_life_max * 0.5) ail = ORANGE;
		else if (ai_life <= ai_life_max * 0.2) ail = RED;
		shape.rectangle(window, pl, pl, Vector2f(player_life * player_life_ratio, 20.f), Vector2f(0.f, 420.f), 0.f, 0.f);
		shape.rectangle(window, SKYBLUE, SKYBLUE, Vector2f(player_strength * player_strength_ratio, 20.f), Vector2f(0.f, 460.f), 0.f, 0.f);
		shape.rectangle(window, ail, ail, Vector2f(ai_life * ai_life_ratio, 20.f), Vector2f(1000.f - ai_life * ai_life_ratio, 420.f), 0.f, 0.f);
		shape.rectangle(window, SKYBLUE, SKYBLUE, Vector2f(ai_strength * ai_strength_ratio, 20.f), Vector2f(1000.f - ai_strength * ai_strength_ratio, 460.f), 0.f, 0.f);
	}
	void level_words() {
		String string = "Level " + std::to_string(level);
		shape.words(window, consola, BLACK, WHITE, string, Vector2f(450.f, 0.f), 30, 0.f, 0.f);
	}
	void winning_words() {
		if (winning) {
			shape.words(window, consola, GOLD, WHITE, "You win!Press Esc to close or F12 to play again.", Vector2f(50.f, 200.f), 30, 0.f, 0.f);
		}
		if (losing) {
			shape.words(window, consola, RED, WHITE, "You lose!Press Esc to close or F12 to play again.", Vector2f(50.f, 200.f), 30, 0.f, 0.f);
		}
	}
	void life_words() {
		String player_life_words = std::to_string(std::lround(player_life)) + "/" + std::to_string(std::lround(player_life_max));
		String ai_life_words = std::to_string(std::lround(ai_life)) + "/" + std::to_string(std::lround(ai_life_max));
		String player_strength_words = std::to_string(std::lround(player_strength)) + "/" + std::to_string(std::lround(player_strength_max));
		String ai_strength_words = std::to_string(std::lround(ai_strength)) + "/" + std::to_string(std::lround(ai_strength_max));
		shape.words(window, consola, BLACK, BLACK, player_life_words, Vector2f(70.f, 420.f), 18, 0.f, 0.f);
		shape.words(window, consola, BLACK, BLACK, player_strength_words, Vector2f(70.f, 460.f), 18, 0.f, 0.f);
		shape.words(window, consola, BLACK, BLACK, ai_life_words, Vector2f(870.f, 420.f), 18, 0.f, 0.f);
		shape.words(window, consola, BLACK, BLACK, ai_strength_words, Vector2f(870.f, 460.f), 18, 0.f, 0.f);
	}
	void player_attack_shape() {
		// 肘击
		player_attack_time = player_attack_clock.getElapsedTime();
		if (player_attack_time.asMilliseconds() <= es_time and player_is == 1) {
			if (ai.x < player.x) {
				Vector2f es_start(player.x - 20.f, player.y - 70.f);
				Vector2f es_middle(player.x - 30.f, player.y - 60.f);
				Vector2f es_end(player.x - 20.f, player.y - 50.f);
				shape.linep(window, GOLD, es_start, es_middle, 3.f);
				shape.linep(window, GOLD, es_middle, es_end, 3.f);
			}
			else if (player.x <= ai.x) {
				Vector2f es_start(player.x + 20.f, player.y - 70.f);
				Vector2f es_middle(player.x + 30.f, player.y - 60.f);
				Vector2f es_end(player.x + 20.f, player.y - 50.f);
				shape.linep(window, GOLD, es_start, es_middle, 3.f);
				shape.linep(window, GOLD, es_middle, es_end, 3.f);
			}
		}
		else if (player_attack_time.asMilliseconds() > es_time) {
			player_is = 0;
			player_is_attack = 0;
		}
	}
	void ai_attack_shape() {
		// 肘击
		ai_attack_time = ai_attack_clock.getElapsedTime();
		if (ai_attack_time.asMilliseconds() <= es_time and ai_is == 1) {
			if (player.x < ai.x) {
				Vector2f es_start(ai.x - 20.f, ai.y - 70.f);
				Vector2f es_middle(ai.x - 30.f, ai.y - 60.f);
				Vector2f es_end(ai.x - 20.f, ai.y - 50.f);
				shape.linep(window, GOLD, es_start, es_middle, 3.f);
				shape.linep(window, GOLD, es_middle, es_end, 3.f);
			}
			else if (ai.x <= player.x) {
				Vector2f es_start(ai.x + 20.f, ai.y - 70.f);
				Vector2f es_middle(ai.x + 30.f, ai.y - 60.f);
				Vector2f es_end(ai.x + 20.f, ai.y - 50.f);
				shape.linep(window, GOLD, es_start, es_middle, 3.f);
				shape.linep(window, GOLD, es_middle, es_end, 3.f);
			}
		}
		else if (ai_attack_time.asMilliseconds() > es_time) {
			ai_is = 0;
			ai_is_attack = 0;
		}
	}
	void background() {
		if (not winning and not losing) {
			shape.pictures(window, bg, Vector2f(1000.f, 400.f), Vector2f(0.f, 0.f));
		}
	}
};



// 主函数
int main() {
	// 设置帧率
	window.setFramerateLimit(FPS);
	// 绘画体
	Element element;
	// 为资源设置路径
	consola.loadFromFile("Fonts/consola.ttf");
	es_music.loadFromFile("Sounds/es.wav");
	hurt_music.loadFromFile("Sounds/hurt.wav");
	bgm.openFromFile(bgm_path[0]);
	bg.loadFromFile("Pictures/background.png");
	// 设置使用资源的资源
	es_sound.setBuffer(es_music);
	es_sound.setVolume(80);
	hurt_sound.setBuffer(hurt_music);
	hurt_sound.setVolume(80);
	bgm.setLoop(false);
	bgm.setVolume(80);
	bgm.play();
	// 主循环
	while (window.isOpen()) {
		// 处理事件
		Event event;
		while (window.pollEvent(event)) {
			// 关闭
			if (event.type == Event::Closed) {
				window.close();
			}
			if (event.type == Event::KeyPressed) {
				if (event.key.code == Keyboard::S and player.y < 400.f and not winning and not losing) {
					player_jumping_height = -15.f;  // 下坠
					player_movement += 50.f;
				}
				if (event.key.code == Keyboard::Escape) window.close();
				if (event.key.code == Keyboard::F12 and (winning or losing)) {
					losing = false;
					winning = true;
					level = 0;
				}
			}
			if (event.type == Event::KeyReleased) {
				// 重置标志
				if (event.key.code == Keyboard::A or event.key.code == Keyboard::D) {
					ai_can_move = true;  // 若按键松开则重置标志以准备下一次重置时钟
				}
			}
			if (event.type == Event::MouseButtonPressed) {
				// 攻击
				if (event.mouseButton.button == Mouse::Left and player_is == 0 and player_strength >= es_strength and not winning and not losing) {
					player_is = 1;
					player_is_attack = 1;
					player_attack_clock.restart();
					player_strength -= es_strength;
					es_sound.play();
				}
			}
		}
		// 玩家移动
		if (Keyboard::isKeyPressed(Keyboard::A) and player.x > 10.f and not winning and not losing) {
			player.x -= player_speed;
			player_movement += player_speed;
			// 重置AI延迟时钟
			if (ai_can_move) {
				ai_sports_delay.restart();
				ai_can_move = false;  // 避免再次重置
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::D) and player.x < 990.f and not winning and not losing) {
			player.x += player_speed;
			player_movement += player_speed;
			// 重置AI延迟时钟
			if (ai_can_move) {
				ai_sports_delay.restart();
				ai_can_move = false;  // 避免再次重置
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::W) and not player_is_jumping and not winning and not losing) player_jumping_bool = true;
		
		// AI移动
		ai_sports_delay_current = ai_sports_delay.getElapsedTime();  // 获取当前时间
		if (ai_sports_delay_current.asMilliseconds() >= ai_sports_delay_length and not winning and not losing) {  // 比较
			if (player.x < ai.x and ai.x > 10.f) {
				ai.x -= ai_speed;
				ai_movement += ai_speed;
			}
			if (ai.x < player.x and ai.x < 990.f) {
				ai.x += ai_speed;
				ai_movement += ai_speed;
			}
		}
		// 生成跳跃随机
		ai_jumping_num = ai_jumping_range(gen);  // 生成随机数
		x_difference = std::abs(player.x - ai.x);
		if (x_difference <= 40.f and ai_jumping_num <= 40 and not ai_is_jumping and not winning and not losing) {
			ai_jumping_bool = true;
			// std::cout << ai_jumping_num << std::endl;
		}
		else if (x_difference > 40.f and ai_jumping_num <= 5 and not ai_is_jumping and not winning and not losing) {
			ai_jumping_bool = true;
			// std::cout << ai_jumping_num << std::endl;
		}
		// 跳跃力度更新
		if (player_jumping_bool and not winning and not losing) {
			player_jumping_height = jumping_height;
			player_jumping_bool = false;
			player_movement += 20.f;
		}
		if (ai_jumping_bool and not winning and not losing) {
			ai_jumping_height = jumping_height;
			ai_jumping_bool = false;
			ai_movement += 20.f;
		}
		// 实时对玩家施加跳跃力度
		if (not winning and not losing) {
			player.y -= player_jumping_height;
			ai.y -= ai_jumping_height;
		}
		// 玩家高度更新
		if (player.y < 400.f and not winning and not losing) {
			player_jumping_height -= G;
			// 在空中对玩家施加空气阻力
			player_speed -= AR;
			player_is_jumping = true;
		}
		if (ai.y < 400.f and not winning and not losing) {
			ai_jumping_height -= G;
			// 在空中对AI施加空气阻力
			ai_speed -= AR;
			ai_is_jumping = true;
		}
		// 攻击
		// 生成攻击随机
		if (ai_is == 0 and not winning and not losing) {
			int ai_attack_num = ai_attack_range(gen);
			// 肘击
			if (ai_attack_num <= 200 and abs(ai.x - player.x) <= 60 and abs(player.y - ai.y) <= 100 and ai_strength >= 1) {
				ai_is = 1;
				ai_is_attack = 1;
				ai_attack_clock.restart();
				ai_strength -= es_strength;
				es_sound.play();
			}
		}
		// 玩家对Ai的攻击
		// 肘击
		if (player_is == 1 and player_is_attack == 1) {
			if (std::abs(player.x - ai.x) <= 40.f and (player.y - ai.y <= 60.f or ai.y - player.y <= 40.f)) {
				boom_num = boom(gen);
				if (boom_num <= 20) {
					ai_life -= es_life * boom_times;
				}
				else {
					ai_life -= es_life;
				}
				if (player.x <= ai.x) ai.x += es_go_away.x;
				if (player.x >= ai.x) ai.x -= es_go_away.x;
				if (player.y <= ai.y) ai.y += es_go_away.y;
				if (player.y >= ai.y) ai.y -= es_go_away.y;
				hurt_sound.play();
				player_is_attack = 0;
			}
		}
		// AI对玩家的攻击
		// 肘击
		if (ai_is == 1 and ai_is_attack == 1) {
			if (std::abs(ai.x - player.x) <= 40.f and (ai.y - player.y <= 60.f or player.y - ai.y <= 40.f)) {
				boom_num = boom(gen);
				if (boom_num <= 20) {
					player_life -= es_life * boom_times;
				}
				else {
					player_life -= es_life;
				}
				if (ai.x <= player.x) player.x += es_go_away.x;
				if (ai.x >= player.x) player.x -= es_go_away.x;
				if (ai.y <= player.y) player.y += es_go_away.y;
				if (ai.y >= player.y) player.y -= es_go_away.y;
				hurt_sound.play();
				ai_is_attack = 0;
			}
		}
		// 避免速度为负数导致后退
		if (player_speed <= 0.f) {
			player_speed = 0.f;
		}
		if (ai_speed <= 0.f) {
			ai_speed = 0.f;
		}
		// 避免玩家掉在地板以下
		if (player.y >= 400.f) {
			player.y = 400.f;
			player_jumping_height = 0.f;
			player_speed = speed;
			player_is_jumping = false;
		}
		if (ai.y >= 400.f) {
			ai.y = 400.f;
			ai_jumping_height = 0.f;
			ai_speed = speed;
			ai_is_jumping = false;
		}
		// 避免玩家穿墙
		if (player.x <= 10.f) {
			player.x = 10.f;
		}
		else if (player.x >= 990.f) {
			player.x = 990.f;
		}
		if (ai.x <= 10.f) {
			ai.x = 10.f;
		}
		else if (ai.x >= 990.f) {
			ai.x = 990.f;
		}
		// 判断movement的扣除体力
		if (player_movement >= movement_max) {
			player_strength -= 1;
			player_movement -= movement_max;
		}
		if (ai_movement >= movement_max) {
			ai_strength -= 1;
			ai_movement -= movement_max;
		}
		// 胜利
		if (ai_life <= 0 or ai_strength <= 0) {
			ai_life = 0;
			ai_strength = 0;
			winning = true;
		}
		// 失败
		if (player_life <= 0 or player_strength <= 0) {
			player_life = 0;
			player_strength = 0;
			losing = true;
		}
		// 升级
		if (winning and level < level_max) {
			level += 1;  // 升级
			winning = false;// 重置
			losing = false;
			// 玩家坐标
			player = Vector2f(200.f, 200.f);
			// AI坐标
			ai = Vector2f(800.f, 200.f);
			// 设置AI移动延迟
			ai_sports_delay.restart();
			ai_sports_delay_current = ai_sports_delay.getElapsedTime();
			ai_can_move = true;
			// 速度
			player_speed = speed;
			ai_speed = speed;
			// 玩家跳跃标志
			player_jumping_bool = false;
			ai_jumping_bool = false;
			player_is_jumping = false;
			ai_is_jumping = false;
			// 移动量
			player_movement = 0.f;
			ai_movement = 0.f;
			// 生命值
			player_life_max = 80.f + 20.f * level;
			ai_life_max = 50.f + 30.f * level;
			player_life = player_life_max;
			ai_life = ai_life_max;
			player_life_ratio = 200.f / player_life_max;
			ai_life_ratio = 200.f / ai_life_max;
			// 体力
			player_strength_max = 5.f + 5.f * level;
			ai_strength_max = 8.f + 5.f * level;
			player_strength = player_strength_max;
			ai_strength = ai_strength_max;
			player_strength_ratio = 200.f / player_strength_max;
			ai_strength_ratio = 200.f / ai_strength_max;
			// 设置攻击标志  // 0:无  1:肘击
			player_is = 0;
			player_is_attack = 0;
			ai_is = 0;
			ai_is_attack = 0;
			// 设置攻击持续的时钟
			player_attack_clock.restart();
			player_attack_time = player_attack_clock.getElapsedTime();
			ai_attack_clock.restart();
			ai_attack_time = ai_attack_clock.getElapsedTime();
		}
		// 检测是否需要切换bgm
		bgm_status = bgm.getStatus();
		if (bgm_status == SoundSource::Stopped) {
			int bgm_switch_num = bgm_switch(gen);
			std::cout << bgm_switch_num << std::endl;
			bgm.openFromFile(bgm_path[bgm_switch_num]);
			bgm.play();
		}

		// 清屏
		window.clear(WHITE);

		// 绘制
		element.background();
		element.floor();
		element.I_man(BLACK, player);
		element.I_man(GREEN, ai);
		element.situation_part();
		element.life_line();
		element.level_words();
		element.life_words();
		element.player_attack_shape();
		element.ai_attack_shape();
		element.winning_words();

		// 刷新
		window.display();
	}
	// 再次按下ESC的提示
	for (int i = 0; i < 33; i++) {
		std::cout << "请再次按下ESC键或关闭此窗口。" << std::endl;
	}
}

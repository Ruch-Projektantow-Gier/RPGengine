namespace rpg {
    class Game {
	public:
        virtual void updateECS(float deltaTime) = 0;
    };
}

#if defined(_WIN32) && defined (RPGENGINE_DLL)
#    ifdef RPGENGINE_EXPORT
//       Compiling a Windows DLL
#        define RPGENGINE_API __declspec(dllexport)
#    else
//       Using a Windows DLL
#        define RPGENGINE_API __declspec(dllimport)
#    endif
// Windows or Linux static library, or Linux so
#else
#define RPGENGINE_API
#endif

RPGENGINE_API void run(rpg::Game* game);

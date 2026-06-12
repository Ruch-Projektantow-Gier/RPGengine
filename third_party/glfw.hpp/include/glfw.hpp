#pragma once
// C++ wrapper for glfw library.

#include <GLFW/glfw3.h>
#include <cstring>
#include <exception>

namespace glfw {
    
    // Version

    struct Version {
        int major, minor, revision;

        constexpr Version(int major_, int minor_, int revision_) :
            major(major_), minor(minor_), revision(revision_) { }

        Version() : major(), minor(), revision() {
            glfwGetVersion(&major, &minor, &revision);
        }
        static constexpr int Major = GLFW_VERSION_MAJOR;
        static constexpr int Minor = GLFW_VERSION_MINOR;
        static constexpr int Revision = GLFW_VERSION_REVISION;

        static const char* getString() { return glfwGetVersionString(); }
        static Version get() { return Version(); }
    };
    inline constexpr Version version = Version(
        Version::Major, Version::Minor, Version::Revision
    );

    inline Version getVersion() { return Version(); }
    inline const char* getVersionString() { return glfwGetVersionString(); }

    // Error Handling

    enum ErrorCode : int {
        NoError = GLFW_NO_ERROR,
        NotInitialized = GLFW_NOT_INITIALIZED,
        NoCurrentContext = GLFW_NO_CURRENT_CONTEXT,
        InvalidEnum = GLFW_INVALID_ENUM,
        InvalidValue = GLFW_INVALID_VALUE,
        OutOfMemory = GLFW_OUT_OF_MEMORY,
        ApiUnavailable = GLFW_API_UNAVAILABLE,
        VersionUnavailable = GLFW_VERSION_UNAVAILABLE,
        PlatformError = GLFW_PLATFORM_ERROR,
        FormatUnavailable = GLFW_FORMAT_UNAVAILABLE,
        NoWindowContext = GLFW_NO_WINDOW_CONTEXT,
#ifdef GLFW_CURSOR_UNAVAILABLE
        CursorUnavailable = GLFW_CURSOR_UNAVAILABLE,
#endif
#ifdef GLFW_FEATURE_UNAVAILABLE
        FeatureUnavailable = GLFW_FEATURE_UNAVAILABLE,
#endif
#ifdef GLFW_FEATURE_UNIMPLEMENTED
        FeatureUnimplamented = GLFW_FEATURE_UNIMPLEMENTED,
#endif
#ifdef GLFW_PLATFORM_UNAVAILABLE
        PlatformUnavailable = GLFW_PLATFORM_UNAVAILABLE
#endif
    };

    inline ErrorCode getError(const char** description) {
#ifdef __EMSCRIPTEN__ // glfwGetError is undefined in emscripten
        description = nullptr;
        return ErrorCode::NoError;
#else
        return static_cast<ErrorCode>(glfwGetError(description));
#endif
    }

    using ErrorFun = void(*)(ErrorCode, const char*);

    inline GLFWerrorfun setErrorCallback(GLFWerrorfun callback) {
        return glfwSetErrorCallback(callback);
    }

    template <ErrorFun callback> inline void setErrorCallback() {
        setErrorCallback([](int code, const char* description){
            callback(static_cast<ErrorCode>(code), description);
        });
    }

    class Exception : public std::exception {
        ErrorCode _code;
        const char* _description;
    public:
        Exception() : _code(NoError), _description(nullptr) {
            _code = getError(&_description);
        }
        Exception(ErrorCode Code, const char* Description) :
            _code(Code), _description(Description) {}
        
        ErrorCode code() const noexcept {
            return _code;
        }
        const char* what() const noexcept override {
            return _description;
        }
    };

    // Library (de)initialization

#ifdef GLFW_PLATFORM
    enum struct Platform : int {
#ifdef GLFW_ANY_PLATFORM
        Any = GLFW_ANY_PLATFORM,
#endif
#ifdef GLFW_PLATFORM_WIN32
        Win32 = GLFW_PLATFORM_WIN32,
#endif
#ifdef GLFW_PLATFORM_COCOA
        Cocoa = GLFW_PLATFORM_COCOA,
#endif
#ifdef GLFW_PLATFORM_WAYLAND
        Wayland = GLFW_PLATFORM_WAYLAND,
#endif
#ifdef GLFW_PLATFORM_X11
        X11 = GLFW_PLATFORM_X11,
#endif
#ifdef GLFW_PLATFORM_NULL
        Null = GLFW_PLATFORM_NULL,
#endif
    };
    inline bool platformSupported(Platform platform) {
        return glfwPlatformSupported(static_cast<int>(platform)) == GLFW_TRUE;
    }
    inline Platform getPlatform() {
        return static_cast<Platform>(glfwGetPlatform());
    }
    inline void initHint(Platform value) {
        glfwInitHint(GLFW_PLATFORM, static_cast<int>(value));
    }
#endif

#ifdef GLFW_JOYSTICK_HAT_BUTTONS
    enum struct JoystickHatButtons : int {
        False = GLFW_FALSE,
        True = GLFW_TRUE,
    };
    inline void initHint(JoystickHatButtons value) {
        glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, static_cast<int>(value));
    }
#endif

#ifdef GLFW_ANGLE_PLATFORM_TYPE
    enum struct AnglePlatformType : int {
#ifdef GLFW_ANGLE_PLATFORM_TYPE_NONE
        None = GLFW_ANGLE_PLATFORM_TYPE_NONE,
#endif
#ifdef GLFW_ANGLE_PLATFORM_TYPE_OPENGL
        OpenGL = GLFW_ANGLE_PLATFORM_TYPE_OPENGL,
#endif
#ifdef GLFW_ANGLE_PLATFORM_TYPE_OPENGLES
        OpenGLES = GLFW_ANGLE_PLATFORM_TYPE_OPENGLES,
#endif
#ifdef GLFW_ANGLE_PLATFORM_TYPE_D3D9
        D3D9 = GLFW_ANGLE_PLATFORM_TYPE_D3D9,
#endif
#ifdef GLFW_ANGLE_PLATFORM_TYPE_D3D11
        D3D11 = GLFW_ANGLE_PLATFORM_TYPE_D3D11,
#endif
#ifdef GLFW_ANGLE_PLATFORM_TYPE_VULKAN
        Vulkan = GLFW_ANGLE_PLATFORM_TYPE_VULKAN,
#endif
#ifdef GLFW_ANGLE_PLATFORM_TYPE_METAL
        Metal = GLFW_ANGLE_PLATFORM_TYPE_METAL,
#endif
    };
    inline void initHint(AnglePlatformType value) {
        glfwInitHint(GLFW_ANGLE_PLATFORM_TYPE, static_cast<int>(value));
    }
#endif

#ifdef GLFW_COCOA_CHDIR_RESOURCES
    enum struct CocoaChdirResources : int {
        False = GLFW_FALSE,
        True = GLFW_TRUE,
    };
    inline void initHint(CocoaChdirResources value) {
        glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, static_cast<int>(value));
    }
#endif

#ifdef GLFW_COCOA_MENUBAR
    enum struct CocoaMenubar : int {
        False = GLFW_FALSE,
        True = GLFW_TRUE,
    };
    inline void initHint(CocoaMenubar value) {
        glfwInitHint(GLFW_COCOA_MENUBAR, static_cast<int>(value));
    }
#endif

#ifdef GLFW_WAYLAND_LIBDECOR
    enum struct WaylandLibdecor : int {
        Prefer = GLFW_WAYLAND_PREFER_LIBDECOR,
        Disable = GLFW_WAYLAND_DISABLE_LIBDECOR,
    };
    inline void initHint(WaylandLibdecor value) {
        glfwInitHint(GLFW_WAYLAND_LIBDECOR, static_cast<int>(value));
    }
#endif

#ifdef GLFW_X11_XCB_VULKAN_SURFACE
    enum struct X11XcbVulkanSurface : int {
        False = GLFW_FALSE,
        True = GLFW_TRUE,
    };
    inline void initHint(X11XcbVulkanSurface value) {
        glfwInitHint(GLFW_X11_XCB_VULKAN_SURFACE, static_cast<int>(value));
    }
#endif

    inline void init() { if (!glfwInit()) throw Exception(); }
    inline void terminate() { glfwTerminate(); }

    struct GLFW {
        GLFW() { init(); }
        ~GLFW() { terminate(); }
    };

    inline static const GLFW context = GLFW();

    // Events

    inline void pollEvents() { glfwPollEvents(); }
    inline void waitEvents() { glfwWaitEvents(); }
    inline void waitEventsTimeout(double timeout) { glfwWaitEventsTimeout(timeout); }
    inline void postEmptyEvent() { glfwPostEmptyEvent(); }

    // Graphics API specifics

    inline void swapInterval(int interval) { glfwSwapInterval(interval); }
    inline bool extensionSupported(const char *extension) { return glfwExtensionSupported(extension) == GLFW_TRUE; }
    inline GLFWglproc getProcAddress(const char *procname) { return glfwGetProcAddress(procname); }
    inline bool vulkanSupported() { return glfwVulkanSupported() == GLFW_TRUE; }
    inline auto getRequiredInstanceExtensions() {
        struct { const char** extensions; uint32_t count; } ext;
        ext.extensions = glfwGetRequiredInstanceExtensions(&ext.count);
        return ext;
    }

    // Time

    inline double getTime() { return glfwGetTime(); }
    inline void setTime(double time) { return glfwSetTime(time); }
    inline uint64_t getTimerValue() { return glfwGetTimerValue(); }
    inline uint64_t getTimerFrequency() { return glfwGetTimerFrequency(); }

    // Input

    enum struct Key {
        Unknown = GLFW_KEY_UNKNOWN,
        Space = GLFW_KEY_SPACE,
        Apostrophe = GLFW_KEY_APOSTROPHE,
        Comma = GLFW_KEY_COMMA,
        Minus = GLFW_KEY_MINUS,
        Period = GLFW_KEY_PERIOD,
        Slash = GLFW_KEY_SLASH,
        _0 = GLFW_KEY_0,
        _1 = GLFW_KEY_1,
        _2 = GLFW_KEY_2,
        _3 = GLFW_KEY_3,
        _4 = GLFW_KEY_4,
        _5 = GLFW_KEY_5,
        _6 = GLFW_KEY_6,
        _7 = GLFW_KEY_7,
        _8 = GLFW_KEY_8,
        _9 = GLFW_KEY_9,
        Semicolon = GLFW_KEY_SEMICOLON,
        Equal = GLFW_KEY_EQUAL,
        A = GLFW_KEY_A,
        B = GLFW_KEY_B,
        C = GLFW_KEY_C,
        D = GLFW_KEY_D,
        E = GLFW_KEY_E,
        F = GLFW_KEY_F,
        G = GLFW_KEY_G,
        H = GLFW_KEY_H,
        I = GLFW_KEY_I,
        J = GLFW_KEY_J,
        K = GLFW_KEY_K,
        L = GLFW_KEY_L,
        M = GLFW_KEY_M,
        N = GLFW_KEY_N,
        O = GLFW_KEY_O,
        P = GLFW_KEY_P,
        Q = GLFW_KEY_Q,
        R = GLFW_KEY_R,
        S = GLFW_KEY_S,
        T = GLFW_KEY_T,
        U = GLFW_KEY_U,
        V = GLFW_KEY_V,
        W = GLFW_KEY_W,
        X = GLFW_KEY_X,
        Y = GLFW_KEY_Y,
        Z = GLFW_KEY_Z,
        LeftBracket = GLFW_KEY_LEFT_BRACKET,
        Backslash = GLFW_KEY_BACKSLASH,
        RightBracket = GLFW_KEY_RIGHT_BRACKET,
        GraveAccent = GLFW_KEY_GRAVE_ACCENT,
        World1 = GLFW_KEY_WORLD_1,
        World2 = GLFW_KEY_WORLD_2,
        Escape = GLFW_KEY_ESCAPE,
        Enter = GLFW_KEY_ENTER,
        Tab = GLFW_KEY_TAB,
        Backspace = GLFW_KEY_BACKSPACE,
        Insert = GLFW_KEY_INSERT,
        Delete = GLFW_KEY_DELETE,
        Right = GLFW_KEY_RIGHT,
        Left = GLFW_KEY_LEFT,
        Down = GLFW_KEY_DOWN,
        Up = GLFW_KEY_UP,
        PageUp = GLFW_KEY_PAGE_UP,
        PageDown = GLFW_KEY_PAGE_DOWN,
        Home = GLFW_KEY_HOME,
        End = GLFW_KEY_END,
        CapsLock = GLFW_KEY_CAPS_LOCK,
        ScrollLock = GLFW_KEY_SCROLL_LOCK,
        NumLock = GLFW_KEY_NUM_LOCK,
        PrintScreen = GLFW_KEY_PRINT_SCREEN,
        Pause = GLFW_KEY_PAUSE,
        F1 = GLFW_KEY_F1,
        F2 = GLFW_KEY_F2,
        F3 = GLFW_KEY_F3,
        F4 = GLFW_KEY_F4,
        F5 = GLFW_KEY_F5,
        F6 = GLFW_KEY_F6,
        F7 = GLFW_KEY_F7,
        F8 = GLFW_KEY_F8,
        F9 = GLFW_KEY_F9,
        F10 = GLFW_KEY_F10,
        F11 = GLFW_KEY_F11,
        F12 = GLFW_KEY_F12,
        F13 = GLFW_KEY_F13,
        F14 = GLFW_KEY_F14,
        F15 = GLFW_KEY_F15,
        F16 = GLFW_KEY_F16,
        F17 = GLFW_KEY_F17,
        F18 = GLFW_KEY_F18,
        F19 = GLFW_KEY_F19,
        F20 = GLFW_KEY_F20,
        F21 = GLFW_KEY_F21,
        F22 = GLFW_KEY_F22,
        F23 = GLFW_KEY_F23,
        F24 = GLFW_KEY_F24,
        F25 = GLFW_KEY_F25,
        Kp0 = GLFW_KEY_KP_0,
        Kp1 = GLFW_KEY_KP_1,
        Kp2 = GLFW_KEY_KP_2,
        Kp3 = GLFW_KEY_KP_3,
        Kp4 = GLFW_KEY_KP_4,
        Kp5 = GLFW_KEY_KP_5,
        Kp6 = GLFW_KEY_KP_6,
        Kp7 = GLFW_KEY_KP_7,
        Kp8 = GLFW_KEY_KP_8,
        Kp9 = GLFW_KEY_KP_9,
        KpDecimal = GLFW_KEY_KP_DECIMAL, 
        KpDivide = GLFW_KEY_KP_DIVIDE,
        KpMultiply = GLFW_KEY_KP_MULTIPLY,
        KpSubtract = GLFW_KEY_KP_SUBTRACT,
        KpAdd = GLFW_KEY_KP_ADD,
        KpEnter = GLFW_KEY_KP_ENTER,
        KpEqual = GLFW_KEY_KP_EQUAL,
        LeftShift = GLFW_KEY_LEFT_SHIFT,
        LeftControl = GLFW_KEY_LEFT_CONTROL,
        LeftAlt = GLFW_KEY_LEFT_ALT,
        LeftSuper = GLFW_KEY_LEFT_SUPER,
        RightShift = GLFW_KEY_RIGHT_SHIFT,
        RightControl = GLFW_KEY_RIGHT_CONTROL,
        RightAlt = GLFW_KEY_RIGHT_ALT,
        RightSuper = GLFW_KEY_RIGHT_SUPER,
        Menu = GLFW_KEY_MENU,
        Last = GLFW_KEY_LAST
    };

    enum struct MouseButton : int {
        _1 = GLFW_MOUSE_BUTTON_1,
        _2 = GLFW_MOUSE_BUTTON_2,
        _3 = GLFW_MOUSE_BUTTON_3,
        _4 = GLFW_MOUSE_BUTTON_4,
        _5 = GLFW_MOUSE_BUTTON_5,
        _6 = GLFW_MOUSE_BUTTON_6,
        _7 = GLFW_MOUSE_BUTTON_7,
        _8 = GLFW_MOUSE_BUTTON_8,
        Left = GLFW_MOUSE_BUTTON_LEFT,
        Right = GLFW_MOUSE_BUTTON_RIGHT,
        Middle = GLFW_MOUSE_BUTTON_MIDDLE,
        Last = GLFW_MOUSE_BUTTON_LAST
    };

    using Scancode = int;

    inline Scancode getKeyScancode(Key key) {
        return glfwGetKeyScancode(static_cast<int>(key));
    }
    inline const char* getKeyName(Key key) {
        return glfwGetKeyName(static_cast<int>(key), 0);
    }
    inline const char* getKeyName(Scancode scancode) {
        return glfwGetKeyName(GLFW_KEY_UNKNOWN, scancode);
    }

    enum struct Action : int {
        Press = GLFW_PRESS,
        Release = GLFW_RELEASE,
        Repeat = GLFW_REPEAT
    };

    enum struct Modifier : int {
        Shift = GLFW_MOD_SHIFT,
        Control = GLFW_MOD_CONTROL,
        Alt = GLFW_MOD_ALT,
        Super = GLFW_MOD_SUPER,
        CapsLock = GLFW_MOD_CAPS_LOCK,
        NumLock = GLFW_MOD_NUM_LOCK
    };

    struct Modifiers {
        int _value;

        inline explicit Modifiers(int mods) : _value(mods) {}
        inline explicit Modifiers(Modifier mod) : _value(static_cast<int>(mod)) {}

        inline explicit operator bool() { return _value != 0; }

        inline Modifiers& operator|=(Modifier lhs) {
            _value |= static_cast<int>(lhs);
            return *this;
        }
        inline Modifiers& operator&=(Modifier lhs) {
            _value &= static_cast<int>(lhs);
            return *this;
        }
        inline Modifiers& operator|=(Modifiers lhs) {
            _value |= lhs._value;
            return *this;
        }
        inline Modifiers& operator&=(Modifiers lhs) {
            _value &= lhs._value;
            return *this;
        }
        inline friend Modifiers operator|(Modifier rhs, Modifiers lhs) {
            return Modifiers(static_cast<int>(rhs) | lhs._value);
        }
        inline friend Modifiers operator&(Modifier rhs, Modifiers lhs) {
            return Modifiers(static_cast<int>(rhs) | lhs._value);
        }
        inline friend Modifiers operator|(Modifiers rhs, Modifier lhs) {
            return Modifiers(rhs._value | static_cast<int>(lhs));
        }
        inline friend Modifiers operator&(Modifiers rhs, Modifier lhs) {
            return Modifiers(rhs._value | static_cast<int>(lhs));
        }
        inline friend Modifiers operator|(Modifiers rhs, Modifiers lhs) {
            return Modifiers(rhs._value | lhs._value);
        }
        inline friend Modifiers operator&(Modifiers rhs, Modifiers lhs) {
            return Modifiers(rhs._value | lhs._value);
        }
    };

    enum struct Event : int {
        Connected = GLFW_CONNECTED,
        Disconnected = GLFW_DISCONNECTED
    };

    // Joystick

    enum struct Joystick : int {
        _1 = GLFW_JOYSTICK_1,
        _2 = GLFW_JOYSTICK_2,
        _3 = GLFW_JOYSTICK_3,
        _4 = GLFW_JOYSTICK_4,
        _5 = GLFW_JOYSTICK_5,
        _6 = GLFW_JOYSTICK_6,
        _7 = GLFW_JOYSTICK_7,
        _8 = GLFW_JOYSTICK_8,
        _9 = GLFW_JOYSTICK_9,
        _10 = GLFW_JOYSTICK_10,
        _11 = GLFW_JOYSTICK_11,
        _12 = GLFW_JOYSTICK_12,
        _13 = GLFW_JOYSTICK_13,
        _14 = GLFW_JOYSTICK_14,
        _15 = GLFW_JOYSTICK_15,
        _16 = GLFW_JOYSTICK_16,
        Last = GLFW_JOYSTICK_LAST,
    };

    inline bool joystickPresent(Joystick jid) {
        return glfwJoystickPresent(static_cast<int>(jid)) == GLFW_TRUE;
    }
    inline auto getJoystickAxes(Joystick jid) {
        struct { const float* axes; int count; } axes;
        axes.axes = glfwGetJoystickAxes(static_cast<int>(jid), &axes.count);
        return axes;
    }
    inline auto getJoystickButtons(Joystick jid) {
        struct { const unsigned char* buttons; int count; } buttons;
        buttons.buttons = glfwGetJoystickButtons(static_cast<int>(jid), &buttons.count);
        return buttons;
    }
    inline const char* getJoystickName(Joystick jid) {
        return glfwGetJoystickName(static_cast<int>(jid));
    }
    inline void* getJoystickUserPointer(Joystick jid) {
        return glfwGetJoystickUserPointer(static_cast<int>(jid));
    }
    template <typename T> inline T* getJoystickUserPointer(Joystick jid) {
        return reinterpret_cast<T*>(getJoystickUserPointer(jid));
    }
    inline void setJoystickUserPointer(Joystick jid, void* pointer) {
        glfwSetJoystickUserPointer(static_cast<int>(jid), pointer);
    }

    using JoystickFun = void (*)(Joystick, Event);

    inline GLFWjoystickfun setJoystickCallback(GLFWjoystickfun callback) {
        return glfwSetJoystickCallback(callback);
    }
    template <JoystickFun callback> inline void setJoystickCallback() {
        setJoystickCallback([](int jid, int event){
            callback(static_cast<Joystick>(jid), static_cast<Event>(event));
        });
    }

    inline bool joystickIsGamepad(Joystick jid) {
        return glfwJoystickIsGamepad(static_cast<int>(jid)) == GLFW_TRUE;
    }

    template <Joystick joystick>
    struct JoystickType {
        inline static bool present() { return joystickPresent(joystick); }
        inline static auto getButtons() { return getJoystickButtons(joystick); }
        inline static auto getAxes() { return getJoystickAxes(joystick); }
        inline static const char* getName() { return getJoystickName(joystick); }
        inline static void* getUserPointer() { return getJoystickUserPointer(joystick); }
        template <typename T> inline static T* getUserPointer() { return getJoystickUserPointer<T>(joystick); }
        inline static void setUserPointer(void* pointer) { setJoystickUserPointer(joystick, pointer); }
    };
    using Joystick1 = JoystickType<Joystick::_1>;
    using Joystick2 = JoystickType<Joystick::_2>;
    using Joystick3 = JoystickType<Joystick::_3>;
    using Joystick4 = JoystickType<Joystick::_4>;
    using Joystick5 = JoystickType<Joystick::_5>;
    using Joystick6 = JoystickType<Joystick::_6>;
    using Joystick7 = JoystickType<Joystick::_7>;
    using Joystick8 = JoystickType<Joystick::_8>;
    using Joystick9 = JoystickType<Joystick::_9>;
    using Joystick10 = JoystickType<Joystick::_10>;
    using Joystick11 = JoystickType<Joystick::_11>;
    using Joystick12 = JoystickType<Joystick::_12>;
    using Joystick13 = JoystickType<Joystick::_13>;
    using Joystick14 = JoystickType<Joystick::_14>;
    using Joystick15 = JoystickType<Joystick::_15>;
    using Joystick16 = JoystickType<Joystick::_16>;

    struct JoystickObject {
        Joystick joystick;

        JoystickObject(Joystick jid) : joystick(jid) {}
        operator Joystick() { return joystick; }

        bool present() { return joystickPresent(joystick); }
        auto getButtons() { return getJoystickButtons(joystick); }
        auto getAxes() { return getJoystickAxes(joystick); }
        const char* getName() { return getJoystickName(joystick); }
        void* getUserPointer() { return getJoystickUserPointer(joystick); }
        template <typename T> T* getUserPointer() { return getJoystickUserPointer<T>(joystick); }
        void setUserPointer(void* pointer) { setJoystickUserPointer(joystick, pointer); }
    };

    // Gamepad

    inline const char* getGamepadName(Joystick jid) {
        return glfwGetGamepadName(static_cast<int>(jid));
    }

    enum struct GamepadButton : int {
        A = GLFW_GAMEPAD_BUTTON_A,
        B = GLFW_GAMEPAD_BUTTON_B,
        X = GLFW_GAMEPAD_BUTTON_X,
        Y = GLFW_GAMEPAD_BUTTON_Y,
        LeftBumper = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
        RightBumper = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
        Back = GLFW_GAMEPAD_BUTTON_BACK,
        Start = GLFW_GAMEPAD_BUTTON_START,
        Guide = GLFW_GAMEPAD_BUTTON_GUIDE,
        LeftThumb = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
        RightThumb = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
        DpadUp = GLFW_GAMEPAD_BUTTON_DPAD_UP,
        DpadRight = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
        DpadDown = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
        DpadLeft = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,
    };

    enum struct GamepadAxis : int {
        LeftX = GLFW_GAMEPAD_AXIS_LEFT_X,
        LeftY = GLFW_GAMEPAD_AXIS_LEFT_Y,
        RightX = GLFW_GAMEPAD_AXIS_RIGHT_X,
        RightY = GLFW_GAMEPAD_AXIS_RIGHT_Y,
        LeftTrigger = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
        RightTrigger = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,
    };

    struct GamepadState : GLFWgamepadstate {
        float operator[](GamepadAxis axis) const {
            return axes[static_cast<int>(axis)];
        }
        bool operator[](GamepadButton button) const {
            return buttons[static_cast<int>(button)] == GLFW_PRESS;
        }
    };

    inline GamepadState getGamepadState(Joystick jid) {
        GamepadState state;
        if (glfwGetGamepadState(static_cast<int>(jid), &state)) return state;
        else throw Exception();
    }

    inline void updateGamepadMappings(const char* string) {
        glfwUpdateGamepadMappings(string);
    }

    struct Gamepad {
        Joystick joystick;
        const char* getName() { return getGamepadName(joystick); }
        GamepadState getState() { return getGamepadState(joystick); }
    };

    enum struct CursorInputMode : int {
        Normal = GLFW_CURSOR_NORMAL,
        Hidden = GLFW_CURSOR_HIDDEN,
        // Ideal for controlling 3D camera with a mouse
        Disabled = GLFW_CURSOR_DISABLED,
#ifdef GLFW_CURSOR_CAPTURED // sometimes unavailable
        Captured = GLFW_CURSOR_CAPTURED
#endif
    };

    inline bool rawMouseMotionSupported() {
        return glfwRawMouseMotionSupported() == GLFW_TRUE;
    }

    template <typename GLFWobject, typename Wrapper>
    struct WrapperBase {
        inline static Wrapper& getRef(GLFWobject* monitor) {
            return reinterpret_cast<Wrapper&>(*monitor);
        }
        inline static const Wrapper& getRef(const GLFWobject* monitor) {
            return reinterpret_cast<const Wrapper&>(*monitor);
        }

        WrapperBase() = delete;
        WrapperBase(const WrapperBase&) = delete;
        WrapperBase(WrapperBase&&) = delete;
        WrapperBase& operator=(const WrapperBase&) = delete;
        WrapperBase& operator=(WrapperBase&&) = delete;
        ~WrapperBase() = delete;

        GLFWobject* get() { return reinterpret_cast<GLFWobject*>(this); }
        const GLFWobject* get() const { return reinterpret_cast<const GLFWobject*>(this); }

        operator GLFWobject*() { return get(); }
        operator const GLFWobject*() { return get(); }
    };

    template <typename GLFWobject>
    struct GenericWrapper : WrapperBase<GLFWobject, GenericWrapper<GLFWobject>> {
        ~GenericWrapper() = delete;
    };

    // Cursor

    struct Image : GLFWimage {
        Image(int Width, int Height) : GLFWimage {
            Width, Height,
            new unsigned char[Width * Height]
        } {}
        Image(int Width, int Height, unsigned char* Pixels) : Image(Width, Height) {
            std::memcpy(pixels, Pixels, Width * Height);
        }
        Image(const Image& other) : GLFWimage {
            other.width, other.height,
            new unsigned char[other.width * other.height]
        } {
            std::memcpy(pixels, other.pixels, width * height);
        }
        Image(Image&& other) : GLFWimage {
            other.width, other.height, other.pixels
        } {
            other.pixels = nullptr;
        }
        Image& operator=(const Image& other) {
            if (this != &other) {
                width = other.width;
                height = other.height;
                pixels = new unsigned char[width * height];
                std::memcpy(pixels, other.pixels, width * height);
            }
            return *this;
        }
        Image& operator=(Image& other) {
            if (this != &other) {
                width = other.width;
                height = other.height;
                pixels = other.pixels;
                other.pixels = nullptr;
            }
            return *this;
        }
        ~Image() { if (pixels) delete[] pixels; }
    };
    struct ImageView : GLFWimage {
        ImageView(int Width, int Height, unsigned char* Pixels) : GLFWimage {
            Width, Height, Pixels
        } {}
        ImageView(const Image& image) : GLFWimage {
            image.width, image.height, image.pixels
        } {}
        ImageView(const GLFWimage& image) : GLFWimage {
            image.width, image.height, image.pixels
        } {}
    };

    struct Cursor : WrapperBase<GLFWcursor, Cursor> {
        ~Cursor() = delete;

        enum struct Shape : int {
            Arrow = GLFW_ARROW_CURSOR,
            Ibeam = GLFW_IBEAM_CURSOR,
            Crosshair = GLFW_CROSSHAIR_CURSOR,
#ifdef GLFW_POINTING_HAND_CURSOR
            PointingHand = GLFW_POINTING_HAND_CURSOR,
#endif
#ifdef GLFW_RESIZE_EW_CURSOR
            ResizeEW = GLFW_RESIZE_EW_CURSOR,
#endif
#ifdef GLFW_RESIZE_NS_CURSOR
            ResizeNS = GLFW_RESIZE_NS_CURSOR,
#endif
#ifdef GLFW_RESIZE_NWSE_CURSOR
            ResizeNWSE = GLFW_RESIZE_NWSE_CURSOR,
#endif
#ifdef GLFW_RESIZE_NESW_CURSOR
            ResizeNESW = GLFW_RESIZE_NESW_CURSOR,
#endif
#ifdef GLFW_RESIZE_ALL_CURSOR
            ResizeAll = GLFW_RESIZE_ALL_CURSOR,
#endif
#ifdef GLFW_NOT_ALLOWED_CURSOR
            NotAllowed = GLFW_NOT_ALLOWED_CURSOR,
#endif
            HResize = GLFW_HRESIZE_CURSOR,
            VResize = GLFW_VRESIZE_CURSOR,
            Hand = GLFW_HAND_CURSOR,
        };

        inline static Cursor& create(ImageView image, int xhot = 0, int yhot = 0) {
            return getRef(glfwCreateCursor(&image, xhot, yhot));
        }
        void destroy() { glfwDestroyCursor(get()); }
        inline static Cursor& create(Shape shape) {
            return getRef(glfwCreateStandardCursor(static_cast<int>(shape)));
        }
    };


    inline Cursor& createCursor(ImageView image, int xhot = 0, int yhot = 0) {
        return Cursor::create(image, xhot, yhot);
    }
    inline void destroyCursor(Cursor& cursor) { cursor.destroy(); }

    inline Cursor& createStandardCursor(Cursor::Shape shape) { return Cursor::create(shape); }

    // Monitor

    using VideoMode = GenericWrapper<GLFWvidmode>;
    using GammaRamp = GenericWrapper<GLFWgammaramp>;

    struct Monitor;

    using MonitorFun = void (*)(Monitor&, Event);

    struct Monitor : WrapperBase<GLFWmonitor, Monitor> {
        ~Monitor() = delete;

        static Monitor& getPrimary() { return getRef(glfwGetPrimaryMonitor()); }
        inline static auto getMonitors() {
            struct { GLFWmonitor** monitors; int count; } monitors;
            monitors.monitors = glfwGetMonitors(&monitors.count);
            return monitors;
        }
        inline static GLFWmonitorfun setCallback(GLFWmonitorfun callback) {
            return glfwSetMonitorCallback(callback);
        }
        template <MonitorFun callback> inline static GLFWmonitorfun setCallback() {
            return setMonitorCallback([](GLFWmonitor* monitor, int event){
                callback(Monitor::getRef(monitor), static_cast<Event>(event));
            });
        }

        const VideoMode& getVideoMode() { return VideoMode::getRef(glfwGetVideoMode(get())); }
        auto getPhysicalSize() {
            struct { int widthMM; int heightMM; } size;
            glfwGetMonitorPhysicalSize(get(), &size.widthMM, &size.heightMM);
            return size;
        }
        auto getContentScale() {
            struct { float x, y; } scale;
            glfwGetMonitorContentScale(get(), &scale.x, &scale.y);
            return scale;
        }
        auto getPos() {
            struct { int x, y; } position;
            glfwGetMonitorPos(get(), &position.x, &position.y);
            return position;
        }
        auto getWorkarea() {
            struct { int xpos, ypos, width, height; } area;
            glfwGetMonitorWorkarea(get(),
                &area.xpos, &area.ypos,
                &area.width, &area.height
            );
            return area;
        }
        const char* getName() { return glfwGetMonitorName(get()); }

        void setUserPointer(void* pointer) { glfwSetMonitorUserPointer(get(), pointer); }
        void* getUserPointer() { return glfwGetMonitorUserPointer(get()); }
        template <typename T> T* getUserPointer() { return reinterpret_cast<T*>(getUserPointer()); }

        void setGammaRamp(GammaRamp& ramp) {
            glfwSetGammaRamp(get(), ramp.get());
        }
        const GammaRamp& getGammaRamp() {
            return GammaRamp::getRef(glfwGetGammaRamp(get()));
        }
        void setGamma(double gamma) { glfwSetGamma(get(), gamma); }
    };

    inline GLFWmonitorfun setMonitorCallback(GLFWmonitorfun callback) {
        return Monitor::setCallback(callback);
    }

    template <MonitorFun callback> inline GLFWmonitorfun setMonitorCallback() {
        return Monitor::setCallback<callback>();
    }

    inline Monitor& getPrimaryMonitor() { return Monitor::getPrimary(); }

    inline auto getMonitors() { return Monitor::getMonitors(); }

    // Window

    enum ClientApi : int {
        NoApi = GLFW_NO_API,
        OpenGLApi = GLFW_OPENGL_API,
        OpenGLESApi = GLFW_OPENGL_ES_API
    };

    struct Window;

    using KeyFun = void (*)(Window&, Key, Scancode, Action, Modifiers);
    using CharFun = void (*)(Window&, unsigned int);
    using CursorPosFun = void (*)(Window&, double, double);
    using MouseButtonFun = void (*)(Window&, MouseButton, Action, Modifiers);
    using WindowSizeFun = void (*)(Window&, int, int);
    using FramebufferSizeFun = void (*)(Window&, int, int);
    using ScrollFun = void (*)(Window&, double, double);
    using CursorEnterFun = void (*)(Window&, bool);
    using DropFun = void (*)(Window&, const char**);

    struct Window : WrapperBase<GLFWwindow, Window> {
        ~Window() = delete;

        // hints

        enum struct Hint {
            Focused = GLFW_FOCUSED,
            Iconified = GLFW_ICONIFIED,
            Resizable = GLFW_RESIZABLE,
            Visible = GLFW_VISIBLE,
            Decorated = GLFW_DECORATED,
            AutoIconify = GLFW_AUTO_ICONIFY,
            Floating = GLFW_FLOATING,
            Maximized = GLFW_MAXIMIZED,
            CetnerCursor = GLFW_CENTER_CURSOR,
            ClientApi = GLFW_CLIENT_API,
            ContextVersionMajor = GLFW_CONTEXT_VERSION_MAJOR,
            ContextVersionMinor = GLFW_CONTEXT_VERSION_MINOR,
        };


        static void hint(Hint hint, int value) {
            glfwWindowHint(static_cast<int>(hint), value);
        }
        static void hint(Hint hint_, bool value) {
            hint(hint_, value ? GLFW_TRUE : GLFW_FALSE);
        }
        static void hint(ClientApi api) {
            hint(Hint::ClientApi, static_cast<int>(api));
        }

        static Window& create(
            int width, int height, const char* title = "",
            GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr
        ) {
            return getRef(glfwCreateWindow(width, height, title, monitor, share));
        }
        
        // configuration

        void setCursorMode(CursorInputMode mode) {
            glfwSetInputMode(get(), GLFW_CURSOR, static_cast<int>(mode));
        }

        void setStickyMouseButtons(bool value) {
            glfwSetInputMode(get(), GLFW_STICKY_MOUSE_BUTTONS, value ? GLFW_TRUE : GLFW_FALSE);
        }

        void setCursor() { glfwSetCursor(get(), NULL); }
        void setCursor(Cursor& cursor) { glfwSetCursor(get(), cursor); }

        void setRawMouseMotion(bool value) {
            glfwSetInputMode(get(), GLFW_RAW_MOUSE_MOTION, value ? GLFW_TRUE : GLFW_FALSE);
        }

        void destroy() { glfwDestroyWindow(get()); }
        bool shouldClose() { return glfwWindowShouldClose(get()) == GLFW_TRUE; }
        void swapBuffers() { glfwSwapBuffers(get()); }
        void makeContextCurrent() { glfwMakeContextCurrent(get()); }

        // callbacks and querries

        void setUserPointer(void* pointer) { glfwSetWindowUserPointer(get(), pointer); }
        void* getUserPointer() { return glfwGetWindowUserPointer(get()); }
        template <typename T> T* getUserPointer() {
            return reinterpret_cast<T*>(getUserPointer());
        }

        GLFWkeyfun setKeyCallback(GLFWkeyfun callback) {
            return glfwSetKeyCallback(get(), callback);
        }

        template <auto callback> void setKeyCallback() {
            setKeyCallback(
                [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                    callback(
                        getRef(window),
                        static_cast<Key>(key),
                        static_cast<Scancode>(scancode),
                        static_cast<Action>(action),
                        Modifiers(mods)
                    );
                }
            );
        }

        Action getKey(Key key) {
            return static_cast<Action>(glfwGetKey(get(), static_cast<int>(key)));
        }

        GLFWcharfun setCharCallback(GLFWcharfun callback) {
            return glfwSetCharCallback(get(), callback);
        }

        template <CharFun callback> void setCharCallback() {
            setCharCallback([](GLFWwindow* window, unsigned int codepoint){
                callback(getRef(window), codepoint);
            });
        }

        GLFWcursorposfun setCursorPosCallback(GLFWcursorposfun callback) {
            return glfwSetCursorPosCallback(get(), callback);
        }

        template <CursorPosFun callback> void setCursorPosCallback() {
            setCursorPosCallback([](GLFWwindow* window, double x, double y){
                callback(getRef(window), x, y);
            });
        }

        auto getCursorPos() {
            struct { double x; double y; } pos;
            glfwGetCursorPos(get(), &pos.x, &pos.y);
            return pos;
        }

        GLFWmousebuttonfun setMouseButtonCallback(GLFWmousebuttonfun callback) {
            return glfwSetMouseButtonCallback(get(), callback);
        }

        template <MouseButtonFun callback> void setMouseButtonCallback() {
            setMouseButtonCallback([](GLFWwindow* window, int key, int action, int mods){
                callback(
                    getRef(window),
                    static_cast<MouseButton>(key),
                    static_cast<Action>(action),
                    Modifiers(mods)
                );
            });
        }

        Action getMouseButton(MouseButton button) {
            return static_cast<Action>(glfwGetMouseButton(get(), static_cast<int>(button)));
        }

        GLFWwindowsizefun setWindowSizeCallback(GLFWwindowsizefun callback) {
            return glfwSetWindowSizeCallback(get(), callback);
        }

        template <WindowSizeFun callback> void setWindowSizeCallback() {
            setWindowSizeCallback([](GLFWwindow* window, int width, int height){
                callback(getRef(window), width, height);
            });
        }

        auto getSize() {
            struct { int width; int height; } size;
            glfwGetWindowSize(get(), &size.width, &size.height);
            return size;
        }

        void setSize(int width, int height) {
            glfwSetWindowSize(get(), width, height);
        }

        GLFWframebuffersizefun setFramebufferSizeCallback(GLFWframebuffersizefun callback) {
            return glfwSetFramebufferSizeCallback(get(), callback);
        }

        template <FramebufferSizeFun callback> void setFramebufferSizeCallback() {
            setFramebufferSizeCallback([](GLFWwindow* window, int width, int height){
                callback(getRef(window), width, height);
            });
        }

        auto getFramebufferSize() {
            struct { int width; int height; } size;
            glfwGetFramebufferSize(get(), &size.width, &size.height);
            return size;
        }

        GLFWscrollfun setScrollCallback(GLFWscrollfun callback) {
            return glfwSetScrollCallback(get(), callback);
        }

        template <ScrollFun callback> void setScrollCallback() {
            setScrollCallback([](GLFWwindow* window, double x, double y){
                callback(getRef(window), x, y);
            });
        }

        GLFWcursorenterfun setCursorEnterCallback(GLFWcursorenterfun callback) {
            return glfwSetCursorEnterCallback(get(), callback);
        }
        
        template <CursorEnterFun callback> void setCursorEnterCallback() {
            setCursorEnterCallback([](GLFWwindow* window, int entered){
                callback(getRef(window), entered == GLFW_TRUE);
            });
        }

        bool isHovered() {
            return glfwGetWindowAttrib(get(), GLFW_HOVERED) == GLFW_TRUE;
        }

        GLFWdropfun setDropCallback(GLFWdropfun callback) {
            return glfwSetDropCallback(get(), callback);
        }
        template <DropFun callback> void setDropCallback() {
            setDropCallback([](GLFWwindow* window, const char** paths){
                callback(getRef(window), paths);
            });
        }

        const char* getClipboardString() { return glfwGetClipboardString(get()); }
        void setClipboardString(const char* ptr) {
            glfwSetClipboardString(get(), ptr);
        }
    };

    inline const char* getClipboardString() { return glfwGetClipboardString(NULL); }
    inline void setClipboardString(const char* ptr) { glfwSetClipboardString(NULL, ptr); }

    inline Window& createWindow(
        int width, int height, const char* title = "",
        GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr
    ) {
        return Window::create(width, height, title, monitor, share);
    }

    inline void makeContextCurrent(Window& window) { window.makeContextCurrent(); }
    inline Window& getCurrentContext() { return Window::getRef(glfwGetCurrentContext()); }

    namespace raii {
        struct Window {
        private:
            glfw::Window& window;
        public:
            Window(
                int width, int height, const char* title = "",
                GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr
            ) : window(glfw::createWindow(
                width, height, title, monitor, share
            )) {}

            Window(const Window&) = delete;
            Window& operator=(const Window&) = delete;

            ~Window() { window.destroy(); }

            GLFWwindow* get() { return window.get(); }

            void setCursorMode(CursorInputMode mode) { window.setCursorMode(mode); }

            void setRawMouseMotion(bool value) { window.setRawMouseMotion(value); }

            bool shouldClose() { return window.shouldClose(); }

            void setUserPointer(void* pointer) { window.setUserPointer(pointer); }

            void* getUserPointer() { return window.getUserPointer(); }

            template <typename T> T* getUserPointer() { return window.getUserPointer<T>(); }

            GLFWkeyfun setKeyCallback(GLFWkeyfun callback) {
                return window.setKeyCallback(callback);
            }

            template <KeyFun callback> void setKeyCallback() {
                window.setKeyCallback<callback>();
            }

            Action getKey(Key key) { return window.getKey(key); }

            GLFWcharfun setCharCallback(GLFWcharfun callback) {
                return window.setCharCallback(callback);
            }

            template <CharFun callback> void setCharCallback() {
                window.setCharCallback<callback>();
            }

            GLFWcursorposfun setCursorPosCallback(GLFWcursorposfun callback) {
                return window.setCursorPosCallback(callback);
            }

            template <CursorPosFun callback> void setCursorPosCallback() {
                window.setCursorPosCallback<callback>();
            }

            auto getCursorPos() { return window.getCursorPos(); }

            GLFWmousebuttonfun setMouseButtonCallback(GLFWmousebuttonfun callback) {
                return window.setMouseButtonCallback(callback);
            }

            template <MouseButtonFun callback> void setMouseButtonCallback() {
                window.setMouseButtonCallback<callback>();
            }

            Action getMouseButton(MouseButton button) {
                return window.getMouseButton(button);
            }

            GLFWwindowsizefun setWindowSizeCallback(GLFWwindowsizefun callback) {
                return window.setWindowSizeCallback(callback);
            }

            template <WindowSizeFun callback> void setWindowSizeCallback() {
                window.setWindowSizeCallback<callback>();
            }

            void setSize(int width, int height) { window.setSize(width, height); }

            auto getSize() { return window.getSize(); }

            GLFWscrollfun setScrollCallback(GLFWscrollfun callback) {
                return window.setScrollCallback(callback);
            }

            template <ScrollFun callback> void setScrollCallback() {
                window.setScrollCallback<callback>();
            }
        };
        inline Window createWindow(
            int width, int height, const char* title = "",
            GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr
        ) {
            return Window(width, height, title, monitor, share);
        }
    }
}
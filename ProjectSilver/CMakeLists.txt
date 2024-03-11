add_executable(ProjectSilver)

target_compile_features(ProjectSilver PUBLIC cxx_std_23)
target_include_directories(ProjectSilver PRIVATE "include")
target_link_libraries(ProjectSilver PRIVATE SFML::Graphics SFML::Audio SFML::System)
target_precompile_headers(ProjectSilver PRIVATE "include/ProjectSilver/PCHeader.hpp")

set(PROJECT_SILVER_SRC
    "Actor.cpp"
    "AfterImage.cpp"
    "AI.cpp"
    "AnimatedSprite.cpp"
    "Animation.cpp"
    "AssetManager.cpp"
    "AudioEngine.cpp"
    "BasicText.cpp"
    "BoxCollider.cpp"
    "ChaserAI.cpp"
    "Collider.cpp"
    "CollisionMaster.cpp"
    "ConfigHandler.cpp"
    "Drawable.cpp"
    "Effect.cpp"
    "Entity.cpp"
    "FighterAI.cpp"
    "FighterBullet.cpp"
    "GameMaster.cpp"
    "Globals.cpp"
    "GraphicsEngine.cpp"
    "GunTurretAI.cpp"
    "InputHandler.cpp"
    "KnightAI.cpp"
    "LevelDirector.cpp"
    "LogHandler.cpp"
    "Main.cpp"
    "Medkit.cpp"
    "MenuDirector.cpp"
    "MiscUtility.cpp"
    "PCHeader.cpp"
    "Player.cpp"
    "PlayerPseudoAI.cpp"
    "RenderComponent.cpp"
    "RifleAmmoPack.cpp"
    "ShadowAI.cpp"
    "ShadowHPBar.cpp"
    "SphereCollider.cpp"
    "Sprite.cpp"
    "Tracer.cpp"
)
list(TRANSFORM PROJECT_SILVER_SRC PREPEND "src/ProjectSilver/")

target_sources(ProjectSilver PUBLIC ${PROJECT_SILVER_SRC})
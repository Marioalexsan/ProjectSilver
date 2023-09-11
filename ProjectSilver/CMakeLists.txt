add_executable(ProjectSilver)

target_compile_features(ProjectSilver PUBLIC cxx_std_23)
target_include_directories(ProjectSilver PRIVATE "include")
target_link_libraries(ProjectSilver PRIVATE SFML::Graphics SFML::Audio SFML::System)
target_precompile_headers(ProjectSilver PRIVATE "include/ProjectSilver/PCHeader.hpp")
target_sources(ProjectSilver
  PUBLIC
    "src/ProjectSilver/Actor.cpp"
    "src/ProjectSilver/AfterImage.cpp"
    "src/ProjectSilver/AI.cpp"
    "src/ProjectSilver/AnimatedSprite.cpp"
    "src/ProjectSilver/Animation.cpp"
    "src/ProjectSilver/AssetManager.cpp"
    "src/ProjectSilver/AudioEngine.cpp"
    "src/ProjectSilver/BasicText.cpp"
    "src/ProjectSilver/BoxCollider.cpp"
    "src/ProjectSilver/ChaserAI.cpp"
    "src/ProjectSilver/Collider.cpp"
    "src/ProjectSilver/CollisionMaster.cpp"
    "src/ProjectSilver/ConfigHandler.cpp"
    "src/ProjectSilver/Drawable.cpp"
    "src/ProjectSilver/Effect.cpp"
    "src/ProjectSilver/Entity.cpp"
    "src/ProjectSilver/FighterAI.cpp"
    "src/ProjectSilver/FighterBullet.cpp"
    "src/ProjectSilver/GameMaster.cpp"
    "src/ProjectSilver/Globals.cpp"
    "src/ProjectSilver/GraphicsEngine.cpp"
    "src/ProjectSilver/GunTurretAI.cpp"
    "src/ProjectSilver/InputHandler.cpp"
    "src/ProjectSilver/KnightAI.cpp"
    "src/ProjectSilver/LevelDirector.cpp"
    "src/ProjectSilver/LogHandler.cpp"
    "src/ProjectSilver/Main.cpp"
    "src/ProjectSilver/Medkit.cpp"
    "src/ProjectSilver/MenuDirector.cpp"
    "src/ProjectSilver/MiscUtility.cpp"
    
    "src/ProjectSilver/PCHeader.cpp"
    "src/ProjectSilver/Player.cpp"
    "src/ProjectSilver/PlayerPseudoAI.cpp"
    "src/ProjectSilver/RenderComponent.cpp"
    "src/ProjectSilver/RifleAmmoPack.cpp"
    "src/ProjectSilver/ShadowAI.cpp"
    "src/ProjectSilver/ShadowHPBar.cpp"
    "src/ProjectSilver/SphereCollider.cpp"
    "src/ProjectSilver/Sprite.cpp"
    "src/ProjectSilver/Tracer.cpp"
)
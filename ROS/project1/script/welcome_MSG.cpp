# include <gazebo/gazebo.hh>
# include <stdio.h>

namespace gazebo
{
  class WorldPluginProject1 : public WorldPlugin
  {
    public: WorldPluginProject1() : WorldPlugin()
            {
              std::cout << "Welcom to James's World!" << std::endl;
            }
    public: void Load(physics::WorldPtr _world, sdf::ElementPtr _sdf)
            {
            }
  };

  GZ_REGISTER_WORLD_PLUGIN(WorldPluginProject1)
}

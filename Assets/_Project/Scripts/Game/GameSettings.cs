using System.Linq;

namespace Project
{
    public static class GameSettings
    {
        private static SOGameSettings _soGameSettings;

        public static SOGameSettings instance
        {
            get
            {
                if (_soGameSettings == null)
                {
                    _soGameSettings = SOScriptableObjectReferencesCache.GetScriptableObjects<SOGameSettings>().First();
                }

                return _soGameSettings;
            }
        } 
    }
}

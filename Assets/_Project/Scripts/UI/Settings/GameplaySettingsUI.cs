using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using Project.Extensions;
using Project.Scripts.UIFramework;
using UnityEngine;
using Sirenix.OdinInspector;


namespace Project._Project.Scripts.UI.Settings
{
    public class GameplaySettingsUI : MonoBehaviour
    {

#if UNITY_EDITOR
        [ShowInInspector, ShowIf(nameof(_gameplaySettings))] private bool value => _gameplaySettings is { value: true };
#endif

        [SerializeField, Required] private ToggleButton _toggleButton;
        
        [SerializeField, ValueDropdown(nameof(GetGameplaySettingsKey))] private string _key;
        private GameplaySettings _gameplaySettings;
        private IEnumerable<GameplaySettings> _gameplaySettingsFields;



        private IEnumerator Start()
        {
            _gameplaySettings = GetGameplaySettings().First(x => x.key == _key);
            
            // Be sure to execute after the ToggleButton Start method
            yield return null;
            _toggleButton.SetToggle(_gameplaySettings.value);
        }

        private void OnEnable()
        {
            _toggleButton._onToggle.AddListenerExtended(SetValue);
        }
        
        private void OnDisable()
        {
            _toggleButton._onToggle.RemoveListenerExtended(SetValue);
        }
        
        private void SetValue(bool state)
        {
            _gameplaySettings.value = state;
        }

        
        private IEnumerable<GameplaySettings> GetGameplaySettings()
        {
            FieldInfo[] fields = typeof(GameplaySettingsManager).GetFields(BindingFlags.Public | BindingFlags.Static);

            _gameplaySettingsFields = fields
                .Where(field => field.FieldType == typeof(GameplaySettings))
                .Select(x => x.GetValue(null))
                .Cast<GameplaySettings>();

            return _gameplaySettingsFields;
        }
        
        private IEnumerable<string> GetGameplaySettingsKey()
        {
            return GetGameplaySettings().Select(x => x.key);
        }
    }
}
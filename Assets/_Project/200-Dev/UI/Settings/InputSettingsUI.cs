using Project._Project.TESTT_REBIND;
using Sirenix.OdinInspector;
using TMPro;
using UnityEditor;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.UI;
#if UNITY_EDITOR
#endif

namespace Project._Project.Scripts.UI.Settings
{
    public class InputSettingsUI : MonoBehaviour
    {
        [field: Title("Binds")]
        [field: SerializeField, LabelText("Input Action Reference"), OnValueChanged(nameof(UpdateInputBinding))] public InputActionReference inputActionReference { get; private set; }
        [SerializeField, ValueDropdown("@inputActionReference.action.bindings")] private InputBinding _inputBinding;
        [SerializeField, HideInInspector] private int _bindingIndex;
        private string _actionName;
    
        [Title("Visual")]
        [SerializeField] private InputBinding.DisplayStringOptions _displayStringOptions;
        
        [Title("Settings")]
        [SerializeField] private bool _excludeMouse = true;
        [SerializeField] private bool _overrideActionText;
        [SerializeField, ShowIf(nameof(_overrideActionText))] private string _customActionText;
        
        [Title("UI Fields")]
        [SerializeField] private TMP_Text _actionText;
        [SerializeField] private Button _rebindButton;
        [SerializeField] private TMP_Text _rebindText;
        [SerializeField] private Button _resetButton;
        [SerializeField] private Button _clearButton;

        
        private void OnEnable()
        {
            if(inputActionReference != null)
            {
                GetBindingInfo();
                UpdateUI();
            }
            
            _rebindButton.onClick.AddListener(DoRebind);
            _resetButton.onClick.AddListener(ResetBinding);
            _clearButton.onClick.AddListener(ClearRebind);
            
            InputSettingsManager.onRebindComplete += UpdateUI;
            InputSettingsManager.onRebindCanceled += UpdateUI;
        
// #if UNITY_EDITOR
//             if (UnityEngine.SceneManagement.SceneManager.GetActiveScene().name == "Settings")
//             {
//                 Utilities.StartWaitForFramesAndDoActionCoroutine(this, 1, () =>
//                 {
//                     GetBindingInfo();
//                     UpdateUI();
//                 });
//             }
// #endif
        }

        private void OnDisable()
        {
            _rebindButton.onClick.RemoveListener(DoRebind);
            _resetButton.onClick.RemoveListener(ResetBinding);
            _clearButton.onClick.RemoveListener(ClearRebind);
            
            InputSettingsManager.onRebindComplete -= UpdateUI;
            InputSettingsManager.onRebindCanceled -= UpdateUI;
        }

        private void OnValidate()
        {
            #if UNITY_EDITOR
            if (EditorApplication.isPlayingOrWillChangePlaymode) return;
            #endif
            
            if (inputActionReference == null) return; 
            
            GetBindingInfo();
            UpdateUI();
        }
        

        private void UpdateInputBinding()
        {
            _inputBinding = inputActionReference.action.bindings[0];
        }
        
        private void GetBindingInfo()
        {
            if (inputActionReference.action == null) return;
            
            _actionName = inputActionReference.action.name;

            _bindingIndex = inputActionReference.action.bindings.IndexOf(x => x.path == _inputBinding.path);
            if (_bindingIndex == -1) _bindingIndex = 0;
        }

        private void UpdateUI()
        {
            if (_actionText != null)
            {
                _actionText.text = _overrideActionText ? _customActionText : _actionName;
            }

            if (_rebindText != null)
            {
                if (Application.isPlaying)
                {
                    _rebindText.text = InputSettingsManager.GetBindingName(_actionName, _bindingIndex, _displayStringOptions);
                }
                else
                    _rebindText.text = inputActionReference.action.GetBindingDisplayString(_bindingIndex, _displayStringOptions);
            }
        }

        private void DoRebind()
        {
            InputSettingsManager.StartRebind(_actionName, _bindingIndex, _rebindText, _excludeMouse);
        }

        private void ResetBinding()
        {
            InputSettingsManager.ResetBinding(_actionName, _bindingIndex);
            UpdateUI();
        }

        private void ClearRebind()
        {
            InputSettingsManager.ClearBindingOverride(_actionName, _bindingIndex);
            UpdateUI();
        }
    }
}

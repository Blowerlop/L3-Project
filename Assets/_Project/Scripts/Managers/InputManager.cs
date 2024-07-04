using System;
using Sirenix.OdinInspector;
using UnityEditor;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Utilities;
#if UNITY_EDITOR
using Sirenix.OdinInspector.Editor;
#endif

namespace Project
{
    [DefaultExecutionOrder(-1)]
    public class InputManager : MonoSingleton<InputManager>
    {
        #region Variables

        private PlayerInputAction _inputAction;
        public PlayerInputAction inputAction
        {
            get { return _inputAction ??= new PlayerInputAction(); }
        }
        
        [HideInInspector] public string _defaultInputActionMap;
        public InputActionMap currentActionMap { get; private set; }
        public InputActionMap previousActionMap { get; private set; }
        
        [Title("Parameters")]
        [ShowInInspector, PropertyOrder(1), LabelText("Current Action Map")]
        #if UNITY_EDITOR
        private string _currentActionMapEditor
        {
            get
            {
                if (currentActionMap == null) return string.Empty;
                return currentActionMap.name;
            }
        }

        [ShowInInspector, PropertyOrder(1), LabelText("Previous Action Map")]
        private string _previousActionMapEditor
        {
            get
            {
                if (previousActionMap == null) return string.Empty;
                return previousActionMap.name;
            }
        }
#endif
        
        
        [Title("Inputs Handler")]
        public InputAction onMouseButton0 => inputAction.Player.MouseButton0;
        public InputAction onMouseButton1 => inputAction.Player.MouseButton1;
        public InputAction onConsoleKey => inputAction.Persistant.Console;
        public InputAction onCenterCamera => inputAction.Player.CenterCamera;
        public InputAction onLockCamera => inputAction.Player.LockCamera;
        public InputAction onEscapeKey => inputAction.Persistant.Escape;
        public InputAction onEmotesWheel => inputAction.Player.EmotesWheel;
        public InputAction onCancellation => inputAction.Player.Cancellation;

        // Spells
        private InputAction[] _spellInputs;
        public event Action<int> OnSpellInputStarted; 
        public event Action<int> OnOnSpellInputCanceled; 
        
        [Title("Inputs value")] 
        [ShowInInspector] public Vector2 move;
        [ShowInInspector] public Vector2 look;
        
        #endregion
        
        
        #region Updates
        protected override void Awake()
        {
            dontDestroyOnLoad = false;
            base.Awake();
            
            InitInputAction();

            SwitchActionMap(_defaultInputActionMap);
            previousActionMap = currentActionMap;
        }

        protected override void OnDestroy()
        {
            base.OnDestroy();
            
            foreach (var spellAction in _spellInputs)
            {
                spellAction.started -= SpellInputStarted;
                spellAction.canceled -= SpellInputCanceled;
            }
        }

        #endregion

        
        #region Methods
        private void InitInputAction()
        {
            // Move
            inputAction.Player.Move.started += context => move = context.ReadValue<Vector2>();
            inputAction.Player.Move.performed += context => move = context.ReadValue<Vector2>();
            inputAction.Player.Move.canceled += _ => move = Vector2.zero;
            
            // Look
            inputAction.Player.Look.started += context => look = context.ReadValue<Vector2>();
            inputAction.Player.Look.performed += context => look = context.ReadValue<Vector2>();
            inputAction.Player.Look.canceled += _ => look = Vector2.zero;
            
            // Spells
            _spellInputs = new[]
            {
                inputAction.Player.Spell1,
                inputAction.Player.Spell2,
                inputAction.Player.Spell3,
                inputAction.Player.Spell4
            };
            
            foreach (var spellAction in _spellInputs)
            {
                spellAction.started += SpellInputStarted;
                spellAction.canceled += SpellInputCanceled;
            }
        }
        
        [ButtonGroup]
        public void Enable()
        {
            SwitchActionMap(currentActionMap);
            Debug.Log("Inputs enabled !");
        }

        [ButtonGroup]
        public void Disable()
        {
            inputAction.Disable();
            Debug.Log("Inputs disabled !");
        }
        
        [Title("Button"),PropertyOrder(0), Button]
        public void SwitchActionMap(string actionMapName)
        {
            InputActionMap actionMap = inputAction.asset.FindActionMap(actionMapName);
            if (actionMap == null)
            {
                Debug.LogError($"There is no action map with the name : {actionMapName}");
                return;
            }

            SwitchActionMap(actionMap);
        }
        
        public void SwitchActionMap(InputActionMap actionMap)
        {
            if (actionMap == null)
            {
                Debug.LogError("Action map is null");
                return;
            }
            if (currentActionMap == actionMap && actionMap.enabled) return;

            inputAction.Disable();

            Debug.Log(currentActionMap != null
                ? $"Switching action map : {currentActionMap.name} To {actionMap.name}"
                : $"Setting action map : {actionMap.name}");

            previousActionMap = currentActionMap;
            currentActionMap = actionMap;

            Utilities.StartWaitForFramesAndDoActionCoroutine(this, 1, () =>
            {
                currentActionMap.Enable();
                inputAction.Persistant.Enable();
            });
        }

        private void SpellInputStarted(InputAction.CallbackContext _)
        {
            int spellIndex = Array.IndexOf(_spellInputs, _.action);
            OnSpellInputStarted?.Invoke(spellIndex);
        }
        
        private void SpellInputCanceled(InputAction.CallbackContext _)
        {
            int spellIndex = Array.IndexOf(_spellInputs, _.action);
            OnOnSpellInputCanceled?.Invoke(spellIndex);
        }
        
        #endregion
    }


    #if UNITY_EDITOR
    [CustomEditor(typeof(InputManager))]
    public class InputManagerEditor : OdinEditor
    {
        private int _index;
        
        public override void OnInspectorGUI()
        {
            base.OnInspectorGUI();
            InputManager t = target as InputManager;
            if (t == null) return;
            
            PlayerInputAction inputAction = t.inputAction;

            SerializedProperty actionMapProperty = serializedObject.FindProperty("_defaultInputActionMap");
            if (actionMapProperty == null) return;
            // EditorGUILayout.PropertyField(actionMapProperty);

            ReadOnlyArray<InputActionMap> actionMaps = inputAction.asset.actionMaps;
            string[] array = new string[actionMaps.Count];
            for (int i = 0; i < array.Length; i++)
            {
                array[i] = actionMaps[i].name;
                if (array[i] == actionMapProperty.stringValue) _index = i;
            }
            
            _index = EditorGUILayout.Popup("Default Action Map", _index, array);
            actionMapProperty.stringValue = array[_index];
            
            serializedObject.ApplyModifiedProperties();
            
            UnityEditorInternal.InternalEditorUtility.RepaintAllViews();
        }
    }
    #endif
}
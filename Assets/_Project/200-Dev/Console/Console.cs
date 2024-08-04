using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using _Project._200_Dev.Application_Management;
using _Project._200_Dev.Logs.LogFileExporter;
using _Project._200_Dev.Managers;
using _Project._200_Dev.Utilities.Objects;
using Sirenix.OdinInspector;
using Sirenix.Utilities;
using TMPro;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.InputSystem;
using UnityEngine.Profiling;
using UnityEngine.UI;

namespace _Project._200_Dev.Console
{
    [DefaultExecutionOrder(-1)]
    public class Console : MonoSingleton<Console>
    {
        private struct LogWrapper
        {
            public string condition;
            // public string stacktrace;
            public LogType logType;
        }
        
        
        #region Variables

        [Title("Console State")]
        [ShowInInspector, ReadOnly] public bool isConsoleEnabled => instance.gameObject.activeSelf;
        [ShowInInspector, ReadOnly] public bool isInputFieldFocus => _inputInputField != null && _inputInputField.isFocused;

        [Title("Parameters")]
        [SerializeField] private Vector2 _fontSizeRange = new(20, 60);
        [SerializeField] private int _maxCharacters = 10000;
        [SerializeField] private int _maxCommandHistory = 50;

        [Title("Dirty")] 
        private readonly List<LogWrapper> _dirtyList = new();
        
        private const int _LOG_COUNT_OFFSET = 24;
        public readonly Dictionary<string, ConsoleCommand> commands = new();
        public string[] commandsName { get; private set; }
        private readonly List<string> _commandHistory = new();
        private int _currentIndex = -1;
        private bool _navigatingThroughHistory;
        
        [TitleGroup("References")]
        [SerializeField, ChildGameObjectsOnly] private ScrollRect _logScrollRect;
        [SerializeField, ChildGameObjectsOnly] private TMP_InputField _logInputField;
        [SerializeField, ChildGameObjectsOnly] private TMP_InputField _inputInputField;
        [SerializeField, ChildGameObjectsOnly] private ConsoleCommandPrediction _commandPrediction;

        #endregion
        
        
        #region Updates

        protected override void Awake()
        {
            ClearConsole();
            Application.logMessageReceived += MakeDirty;
            RetrieveCommandAttribute();
        }

        private void Start()
        {
            commandsName = new string[commands.Count];
            int index = 0;
            foreach (var kvp in commands)
            {
                commandsName[index] = kvp.Key;
                index++;
            }
            commandsName.Sort();

            HideConsoleForced();
            ClearInputField();
            
            InputManager.instance.onConsoleKey.started += OnConsoleKeyStarted_ToggleConsole;
        }

        private void OnEnable()
        {
            _inputInputField.onSubmit.AddListener(ExecuteCommand);
            _inputInputField.onValueChanged.AddListener(OnInputInputFieldValueChanged_Predict);
        }

        private void OnDisable()
        {
            _inputInputField.onSubmit.RemoveListener(ExecuteCommand);
            _inputInputField.onValueChanged.RemoveListener(OnInputInputFieldValueChanged_Predict);
        }

        protected override void OnDestroy()
        {
            Application.logMessageReceived -= MakeDirty;

            if (!ApplicationManager.isQuitting)
            {
                InputManager.instance.onConsoleKey.started -= OnConsoleKeyStarted_ToggleConsole;
            }
        }

        private void Update()
        {
            if (Input.GetKey(KeyCode.LeftControl))
            {
                GameObject currentCurrentSelectedGameObject = EventSystem.current.currentSelectedGameObject;
                if (currentCurrentSelectedGameObject == _logScrollRect.gameObject ||
                    currentCurrentSelectedGameObject == _logInputField.gameObject)
                {
                    IncreaseOrDecreaseLogTextSize();
                }
                else if (Input.GetKeyDown(KeyCode.Backspace) && isInputFieldFocus)
                {
                    DeleteWordShortcut();
                }
            }


            // InputField Related
            if (isInputFieldFocus == false) return;
            
            if (Input.GetKeyDown(KeyCode.Tab))
            {
                if (_commandPrediction.HasAPrediction())
                {
                    AutoCompleteTextWithThePrediction();
                }
            }
            else if (Input.GetKeyDown(KeyCode.UpArrow))
            {
                if (!_navigatingThroughHistory && _commandPrediction.HasAPrediction())
                {
                    _commandPrediction.WriteNextPrediction();
                    MoveCaretToTheEndOfTheText();
                }
                else
                {
                    GotToTheOlderInHistory();
                }
            }
            else if (Input.GetKeyDown(KeyCode.DownArrow))
            {
                if (!_navigatingThroughHistory && _commandPrediction.HasAPrediction())
                {
                    _commandPrediction.WritePreviousPrediction();
                    MoveCaretToTheEndOfTheText();
                }
                else
                {
                    GotToTheRecentInHistory();
                }
            }
        }

        private void LateUpdate()
        {
            if (!_dirtyList.Any()) return;
            
            LogConsole();
            _dirtyList.Clear();
        }

        #endregion


        #region Methods

        #region  Command Relative

        private void ExecuteCommand(string rawInput)
        {
            if (string.IsNullOrEmpty(rawInput)) return;

            string trimString = rawInput.TrimEnd();
            string[] splitInput = trimString.Split(" ", StringSplitOptions.RemoveEmptyEntries);

            Debug.Log($">> {trimString}");
            AddToCommandHistory(trimString);

            // Check if the command exist
            if (commands.TryGetValue(splitInput[0], out ConsoleCommand command))
            {
                // Check if the command have the same number of parameters that the player input
                object[] parameters = new object[command.parametersInfo.Length];

                int commandsSplitInputLength = splitInput.Length - 1;
                if (commandsSplitInputLength > command.parametersInfo.Length || (commandsSplitInputLength < command.parametersInfo.Length && commandsSplitInputLength < command.parametersInfo.Length - command.parametersWithDefaultValue))
                {
                    int commandParametersLength = command.parametersInfo.Length;

                    switch (commandParametersLength)
                    {
                        case 0:
                            Debug.LogError($"This command has no parameter, you pass {commandsSplitInputLength}");
                            break;
                        case 1:
                            Debug.LogError($"This command has {commandParametersLength} parameter, you pass {commandsSplitInputLength}");
                            break;
                        default:
                            Debug.LogError($"This command has {commandParametersLength} parameters, you pass {commandsSplitInputLength}");
                            break;
                    }

                    goto end;
                }
                
                
                // Loop through the player input and try parse the parameters
                for (int i = 0; i < parameters.Length; i++)
                {
                    if (TryParseParameter(i, out object parameterResult) == false) goto end;

                    parameters[i] = parameterResult;
                }

                try
                {
                    command.InvokeMethod(parameters);
                }
                catch (Exception e)
                {
                    Debug.LogError($"Unexpected behaviour : {e}");
                }
            }
            else
            {
                Debug.LogError($"Unknown command '{splitInput[0]}'");
            }
            
            end:
            ClearInputField();
            FocusOnInputField();

            
            
            bool TryParseParameter(int i, out object parameterResult)
            {
                Type parameterType = command.parametersInfo[i].ParameterType;
                
                if (i + 1 >= splitInput.Length)
                {
                    parameterResult = command.parametersInfo[i].DefaultValue;
                    return true;
                }

                string inputParameter = splitInput[i + 1];

                try
                {
                    // The is the only parameter type that is force check because C# does not Parse 0 and 1 as boolean
                    if (parameterType == typeof(bool))
                    {
                        if (bool.TryParse(inputParameter, out bool boolValue))
                        {
                            parameterResult = boolValue;
                        }
                        else
                        {
                            int valueInt = int.Parse(inputParameter);
                            if (valueInt == 0) parameterResult = false;
                            else if (valueInt == 1) parameterResult = true;
                            else throw new FormatException();
                        }
                    }
                    else
                    {
                        parameterResult = Convert.ChangeType(inputParameter, parameterType, CultureInfo.CurrentCulture);
                    }
                }
                catch
                {
                    Debug.LogError($"Unknown parameter '{inputParameter}'. Parameter need to be a {parameterType.Name}");
                    parameterResult = null;
                    return false;
                }

                return true;
            }
        }
        
        private void AddToCommandHistory(string input)
        {
            if (_commandHistory.Count > _maxCommandHistory)
            {
                // Remove the oldest added command
                _commandHistory.RemoveAt(_commandHistory.Count - 1);
            }

            if (_commandHistory.FirstOrDefault() == input) return;
            
            _commandHistory.Insert(0, input);
            _currentIndex = -1;
        }

        public void AddCommand(ConsoleCommand consoleCommand)
        {
            instance.commands.Add(consoleCommand.name, consoleCommand);
        }

        #endregion
        
        #region Used by shortcut
        private void OnConsoleKeyStarted_ToggleConsole(InputAction.CallbackContext _)
        {
            if (isConsoleEnabled) Hide();
            else Show();
        }
        
        private void GotToTheOlderInHistory()
        {
            if (_currentIndex + 1 >= _commandHistory.Count)
            {
                MoveCaretToTheEndOfTheText();
                return;
            }

            _currentIndex++;
            _navigatingThroughHistory = true;
            
            SetTextOfInputInputFieldSilent(_commandHistory[_currentIndex]);
            PredictCurrentInput();
        }
        
        private void GotToTheRecentInHistory()
        {
            if (_currentIndex <= -1)
            {
                return;
            }
            if (_currentIndex <= 0)
            {
                SetTextOfInputInputFieldSilent(string.Empty);
                PredictCurrentInput();
                _currentIndex = -1;
                return;
            }

            _currentIndex--;
            _navigatingThroughHistory = true;
            
            SetTextOfInputInputFieldSilent(_commandHistory[_currentIndex]);
            PredictCurrentInput();
        }
        
        private void IncreaseOrDecreaseLogTextSize()
        {
            _logInputField.pointSize = Mathf.Clamp(_logInputField.pointSize + Input.mouseScrollDelta.y, _fontSizeRange.x,
                _fontSizeRange.y);
        }
        
        private void DeleteWordShortcut()
        {
            int startWordPosition = 0;
            for (int i = _inputInputField.caretPosition - 1; i >= 0; i--)
            { 
                if (_inputInputField.text[i] == ' ')
                {
                    startWordPosition = i;
                    break;
                }
            }
            
            SetTextOfInputInputField(_inputInputField.text.Remove(startWordPosition, _inputInputField.caretPosition - startWordPosition));
        }

        private void AutoCompleteTextWithThePrediction()
        {
            SetTextOfInputInputField(_commandPrediction.currentPrediction);
            // ClearCommandPrediction();
        }
        #endregion
        
        #region Utilities

        private void PredictCurrentInput()
        {
            _commandPrediction.Predict(_inputInputField.text);
        }
        
        public void SetTextOfInputInputField(string text)
        {
            if (string.Equals(_inputInputField.text, text)) return;
            
            _inputInputField.text = text;
            MoveCaretToTheEndOfTheText();
        }
        
        public void SetTextOfInputInputFieldSilent(string text)
        {
            if (_inputInputField.text == text) return;
            
            _inputInputField.SetTextWithoutNotify(text);
            MoveCaretToTheEndOfTheText();
        }
        
        private void MoveCaretToTheStartOfTheText()
        {
            _inputInputField.MoveTextStart(false);
        }
        
        private void MoveCaretToTheEndOfTheText()
        {
            _inputInputField.MoveTextEnd(false);
        }
        
        private void MoveCaretToPosition(int position)
        {
            _inputInputField.caretPosition = position;
        }
        
        private void ClearInputField() => _inputInputField.text = (string.Empty);
        
        public void FocusOnInputField()
        {
            _inputInputField.ActivateInputField();
        }
        
        
        #endregion

        public void ClearConsole()
        {
            _logInputField.text = string.Empty;
        }
        
        private void OnInputInputFieldValueChanged_Predict(string text)
        {
            _commandPrediction.Predict(text);
            _navigatingThroughHistory = false;
            _currentIndex = -1;
        }
        
        [ButtonGroup]
        public void Show()
        {
            if (isConsoleEnabled) return;

            ShowConsoleForced();
        }

        private void ShowConsoleForced()
        {
            instance.gameObject.SetActive(true);
            instance.OnShowConsole();
        }

        private void OnShowConsole()
        {
            instance.FocusOnInputField();

            InputManager.instance.SwitchActionMap("UI");
        }

        [ButtonGroup]
        public void Hide()
        {
            if (isConsoleEnabled == false) return;
            
            HideConsoleForced();
        }

        private void HideConsoleForced()
        {
            instance.gameObject.SetActive(false);

            instance.OnHideConsole();
        }

        private void OnHideConsole()
        {
            InputManager.instance.SwitchActionMap(InputManager.instance.previousActionMap);
        }
        
        private void LogConsole()
        {
            bool setAtBottom = _logScrollRect.verticalNormalizedPosition <= 0.01f;

            int totalSize = 0;
            foreach (LogWrapper logWrapper in _dirtyList)
            {
                totalSize += logWrapper.condition.Length + _LOG_COUNT_OFFSET;
            }
            
            int difference =  _logInputField.text.Length + totalSize - _maxCharacters;
            if (difference < 0) difference = 0;

            string newText = string.Create(_logInputField.text.Length - difference  + totalSize, _dirtyList,
                (span, state) =>
                {
                    int index = 0;
                    
                    // 1. Reconstruct the old log with offset if necessary
                    for (int i = difference; i < _logInputField.text.Length; i++, index++)
                    {
                        span[index] = _logInputField.text[i];
                    }

                    foreach (var logWrapper in state)
                    {
                        // 2.0 Add the new log
                        // <color=#{0}>{1}</color>\n;
                    
                        // 2.1 Pre format
                        // <color=#
                        span[index++] = '<';
                        span[index++] = 'c';
                        span[index++] = 'o';
                        span[index++] = 'l';
                        span[index++] = 'o';
                        span[index++] = 'r';
                        span[index++] = '=';
                        span[index++] = '#';
            
                        // 2.2 Format color
                        string logColorHexadecimal = CustomLogger.GetLogColorHexadecimal(logWrapper.logType);
                        for (int i = 0; i < 6; i++, index++)
                        {
                            span[index] = logColorHexadecimal[i];
                        }
                    
                        // 2.3 Post format
                        // >
                        span[index++] = '>';
                    
                        // 2.4 Format received log
                        for (int i = 0; i < logWrapper.condition.Length; i++, index++)
                        {
                            span[index] = logWrapper.condition[i];
                        }
                    
                        // 2.5 Post format
                        // </color>\n
                        span[index++] = '<';
                        span[index++] = '/';
                        span[index++] = 'c';
                        span[index++] = 'o';
                        span[index++] = 'l';
                        span[index++] = 'o';
                        span[index++] = 'r';
                        span[index++] = '>';
                        span[index++] = '\n';
                    }
                });
            
            _logInputField.text = newText;

            #if UNITY_EDITOR
            if (setAtBottom && gameObject.activeInHierarchy) // Only to disable the StartCoroutine logError in editor
            #else
            if (setAtBottom)
            #endif
            {
                StartCoroutine(Utilities.Utilities.WaitForEndOfFrameAndDoActionCoroutine(() =>
                {
                    {
                        Canvas.ForceUpdateCanvases();
                        _logScrollRect.verticalNormalizedPosition = 0.0f;
                        Canvas.ForceUpdateCanvases();
                    }
                })); 
            }
        }

        private void MakeDirty(string condition, string stacktrace, LogType logType)
        {
            _dirtyList.Add(new LogWrapper
            {
                condition = condition,
                // stacktrace = stacktrace,
                logType = logType
            });
        }
        
        
        // https://github.com/yasirkula/UnityIngameDebugConsole/blob/master/Plugins/IngameDebugConsole/Scripts/DebugLogConsole.cs
        // Implementation of finding attributes sourced from yasirkula's code
        private void RetrieveCommandAttribute()
        {
            Profiler.BeginSample("ConsoleAttributeRetrieving");

#if UNITY_EDITOR || !NETFX_CORE
            string[] ignoredAssemblies = new string[]
            {
                "Unity",
                "System",
                "Mono.",
                "mscorlib",
                "netstandard",
                "TextMeshPro",
                "Microsoft.GeneratedCode",
                "I18N",
                "Boo.",
                "UnityScript.",
                "ICSharpCode.",
                "ExCSS.Unity",
#if UNITY_EDITOR
				"Assembly-CSharp-Editor",
                "Assembly-UnityScript-Editor",
                "nunit.",
                "SyntaxTree.",
                "AssetStoreTools"
#endif
            };
#endif
#if UNITY_EDITOR || !NETFX_CORE
            foreach (Assembly assembly in AppDomain.CurrentDomain.GetAssemblies())
#else
            foreach (Assembly assembly in new Assembly[] { GetType().Assembly })
#endif
            {
#if (NET_4_6 || NET_STANDARD_2_0) && (UNITY_EDITOR || !NETFX_CORE)
                if (assembly.IsDynamic)
                    continue;
#endif

                string assemblyName = assembly.GetName().Name;

#if UNITY_EDITOR || !NETFX_CORE
                if (ignoredAssemblies.Any(a => assemblyName.ToLower().StartsWith(a.ToLower())))
                {
                    continue;
                }
#endif

                try
                {
                    foreach (Type type in assembly.GetExportedTypes())
                    {
                        foreach (MethodInfo method in type.GetMethods(BindingFlags.Static | BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.DeclaredOnly))
                        {
                            foreach (object attribute in method.GetCustomAttributes(typeof(ConsoleCommandAttribute), false))
                            {
                                ConsoleCommandAttribute commandAttribute = (ConsoleCommandAttribute)attribute;
                                if (commandAttribute != null)
                                {
                                    for (int i = 0; i < commandAttribute.commandNames.Length; i++)
                                    {
                                        AddCommand(new ConsoleCommand(commandAttribute.commandNames[i], commandAttribute.description, method));
                                    }
                                    
                                }
                            }
                        }
                    }
                }
                catch (NotSupportedException) { }
                catch (System.IO.FileNotFoundException) { }
                catch (Exception e)
                {
                    Debug.LogError("Error whilst searching for developer console attributes in assembly(" + assemblyName + "): " + e.Message + ".");
                }
            }
            
            Profiler.EndSample();
        }

        #endregion
    }
}

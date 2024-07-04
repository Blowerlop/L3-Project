using System.Collections.Generic;
using System.Linq;
using UnityEngine;

namespace Project
{
    public class LogDisplayer : MonoSingleton<LogDisplayer>
    {
        private struct LogContainer
        {
            public string message;
            public Color color;


            public LogContainer(string message, Color color)
            {
                this.message = message;
                this.color = color;
            }
        }
        
        [Header("GUI Rect")]
        private const int _MARGIN = 8;
        private readonly float _width = Screen.width / 3.0f;
        private float _height;
        private Rect _area; 
        
        private readonly GUIContent _guiContent = new GUIContent();
        private GUIStyle _guiStyle;
        
        private readonly List<LogContainer> _logsContainer = new List<LogContainer>();
        private const float _LOG_SCREEN_TIME = 5.0f;


        protected override void Awake()
        {
            dontDestroyOnLoad = false;
            base.Awake();
            
            Application.logMessageReceived += OnLogMessageReceived_UpdateGUI; 
            
            #if UNITY_STANDALONE
            Enable(false);
            #endif
        }

        protected override void OnDestroy()
        {
            base.OnDestroy();
            
            Application.logMessageReceived -= OnLogMessageReceived_UpdateGUI; 
        }

        
        [ConsoleCommand("logs_display", "Display log on GUI")]
        private static void Enable(bool state)
        {
            instance.enabled = state;
            instance._logsContainer.Clear();
            instance.StopAllCoroutines();
        }
        
        private void OnLogMessageReceived_UpdateGUI(string condition, string _, LogType logType)
        {
#if UNITY_EDITOR
            if (Application.isPlaying == false) return;
#endif
            
            if (logType is LogType.Log or LogType.Warning) return;

            _logsContainer.Add(new LogContainer(condition, CustomLogger.logErrorColor));
            
#if UNITY_EDITOR
            if (Application.isPlaying == false) return;
#endif
            Timer.StartTimerWithCallbackUnscaled(this, _LOG_SCREEN_TIME, () =>
            {
#if UNITY_EDITOR
                if (Application.isPlaying == false) return;
#endif
                
                Utilities.StartWaitForEndOfFrameAndDoActionCoroutine(this, () =>
                {
#if UNITY_EDITOR
                    if (Application.isPlaying == false) return;
#endif
                    
                    _logsContainer.RemoveAt(0);
                });
            });
        }
        
        
        private void OnGUI()
        {
            if (_logsContainer.Any() == false) return;
            
            
            _area = new Rect(_MARGIN, _MARGIN, _width, _height); ;
            _guiStyle = new GUIStyle(GUI.skin.box);
            
            using (new GUILayout.AreaScope(_area, _guiContent, _guiStyle))
            {
                _height = 0;
                
                foreach (LogContainer logContainer in _logsContainer)
                {
                    Rect labelRect = GUILayoutUtility.GetRect(new GUIContent(logContainer.message), "label");
                    _height += labelRect.height;
                    
                    GUI.contentColor = logContainer.color;
                    GUI.Label(labelRect, logContainer.message);
                }
            }
        }
    }
}

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Reflection;
using _Project._200_Dev.Utilities.Extensions;
using JetBrains.Annotations;
using Sirenix.OdinInspector;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

namespace _Project._200_Dev.Console
{
    public class ConsoleCommandPrediction : MonoBehaviour
    {
        private readonly List<string> _predictions = new(6);
        [CanBeNull] [ShowInInspector, ReadOnly] public string currentPrediction { get; private set; }
        private int _currentPredictionIndex;
        private string _input;
        private string[] _splitInput;
        private string _commandInput;
        
        [Title("UI")]
        [SerializeField, ChildGameObjectsOnly] private TMP_Text _inputFieldPredictionPlaceHolder;
        [SerializeField, ChildGameObjectsOnly] private GameObject _gameObject;
        [SerializeField, AssetsOnly] private Button _template;


        public bool HasAPrediction() => !string.IsNullOrEmpty(currentPrediction);
        
        public void Predict(string input)
        {
            ClearPrediction();

            if (string.IsNullOrEmpty(input)) return;
            
            _input = input;
            _splitInput = input.Split(" ", StringSplitOptions.RemoveEmptyEntries);
            _commandInput = _splitInput[0];

            Console.instance.commandsName.ForEach((commandName, index) =>
            {
                if (commandName.StartsWith(_commandInput, true, CultureInfo.InvariantCulture))
                {
                    _predictions.Add(Console.instance.commandsName[index]);
                }
            });

            // allCommandsName.Debug();

            if (!_predictions.Any())
            {
                ClearPrediction();
                return;
            }
            
            _currentPredictionIndex = 0;
            WritePrediction(_input, _currentPredictionIndex, _commandInput, _splitInput);
            DisplayPredictions();
        }

        private void WritePrediction(string input, int predictionIndex, string commandInput, IReadOnlyCollection<string> splitInput)
        {
            currentPrediction = GetPrediction(predictionIndex);
            
#if UNITY_EDITOR
            // Just to make Rider happy :)
            if (currentPrediction == null)
            {
                Debug.LogError("Current prediction is null, it should never happen");
                ClearPrediction();
                return;
            }
#endif

            int inputLength = commandInput.Length;

            string preWriteCommandName = currentPrediction.Substring(0, inputLength);
            string nonWriteCommandName = currentPrediction.Substring(inputLength);

            if (string.IsNullOrEmpty(nonWriteCommandName))
            {
                _inputFieldPredictionPlaceHolder.text = $"<color=#00000000>{input}</color>";
            }
            else
            {
                _inputFieldPredictionPlaceHolder.text = $"<color=#00000000>{preWriteCommandName}</color>{nonWriteCommandName}";
            }

            for (int i = 0; i < Console.instance.commands[currentPrediction].parametersInfo.Length; i++)
            {
                if (splitInput.Count > i + 1) continue;

                ParameterInfo parameterInfo = Console.instance.commands[currentPrediction].parametersInfo[i];
                if (parameterInfo.HasDefaultValue)
                {
                    _inputFieldPredictionPlaceHolder.text += $" {parameterInfo.Name}(Optional)";
                }
                else
                {
                    _inputFieldPredictionPlaceHolder.text += $" {parameterInfo.Name}";
                }
            }
        }
        
        public void WriteNextPrediction()
        {
            if (_currentPredictionIndex >= _predictions.Count - 1) return;
            
            WritePrediction(_input, ++_currentPredictionIndex, _commandInput, _splitInput);
        }
        
        public void WritePreviousPrediction()
        {
            if (_currentPredictionIndex <= 0) return;
            
            WritePrediction(_input, --_currentPredictionIndex, _commandInput, _splitInput);
        }

        
        private void DisplayPredictions()
        {
            for (var i = 0; i < _predictions.Count; i++)
            {
                var predictionsName = _predictions[i];
                Button instance = Instantiate(_template, _gameObject.transform);
                var predictionIndex = i;
                instance.onClick.AddListener(() =>
                {
                    Console.instance.SetTextOfInputInputFieldSilent(predictionsName);
                    WritePrediction(predictionsName, predictionIndex, predictionsName,
                        new[] { predictionsName });
                    Console.instance.FocusOnInputField();
                });
                instance.GetComponentInChildren<TMP_Text>().text = predictionsName;
            }
        }

        private void ClearPrediction()
        {
            if (currentPrediction == null) return;
            
            currentPrediction = null;
            _predictions.Clear();
            _inputFieldPredictionPlaceHolder.text = string.Empty;
            _gameObject.DestroyChildren();
        }

        private string GetPrediction(int index)
        {
            return _predictions[index];
        }
    }
}

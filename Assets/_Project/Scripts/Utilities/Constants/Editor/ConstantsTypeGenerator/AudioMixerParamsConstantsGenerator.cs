using System;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using Project.Extensions;
using Sirenix.Utilities;
using UnityEditor;
using UnityEngine;
using UnityEngine.Audio;

namespace _Project.ConstantGenerator
{
    public class AudioMixerParamsConstantsGenerator : IConstantGenerator
    {
        public string className { get; set; } = "AudioMixerParams";
        
        
        public void Write(IndentedTextWriter streamWriter)
        {
            foreach (string exposedParametersName in GetExposedParametersName())
            {
                ConstantsGenerator.WriteSummary(streamWriter, exposedParametersName);
                streamWriter.WriteLine($"public const string {exposedParametersName.ConvertToValidIdentifier()} = \"{exposedParametersName}\";");
            }
        }

        private string[] GetExposedParametersName()
        {
            HashSet<string> exposedParametersName = new HashSet<string>();
            
            string[] guids = AssetDatabase.FindAssets("t:AudioMixerController");
            
            foreach (string guid in guids)
            {
                string assetPath = AssetDatabase.GUIDToAssetPath(guid);
                AudioMixer audioMixer = AssetDatabase.LoadAssetAtPath<AudioMixer>(assetPath);
                
                
                // ReSharper disable once PossibleNullReferenceException
                Array exposedParameters = (Array)audioMixer.GetType().GetProperty("exposedParameters").GetValue(audioMixer, null);
                for (int i = 0; i< exposedParameters.Length; i++)
                {
                    object parameter = exposedParameters.GetValue(i);
                    string parameterName = (string)parameter.GetType().GetField("name").GetValue(parameter);
                    exposedParametersName.Add(parameterName);
                }
            }

            return exposedParametersName.ToArray();
        }
    }
}
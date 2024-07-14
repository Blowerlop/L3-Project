using System;
using _Project._200_Dev.Entities.Player.Stats;
using UnityEngine;

namespace Project
{
    [Serializable]
    public class SpellDescription
    {
        [TextArea(1, 5)] public string descriptionText;
        [SerializeReference] public Effect[] effects;
        public string GenerateText()
        {
            string result = string.Empty;
            string[] value = new string[effects.Length * 2];
            for (int i = 0; i < effects.Length ; i++)
            {
                value[i * 2] = effects[i].GetEffectValue().ToString();
                value[i * 2 + 1] = effects[i].GetEffectDuration().ToString();
            }
            if (value == null || effects.Length > value.Length)
            {
                string[] tempValue = new string[effects.Length];
                for (int i = 0; i < tempValue.Length; i++)
                {
                    if (value != null && i < value.Length)
                        tempValue[i] = value[i];
                    else
                        tempValue[i] = "#MISSING_VALUE";
                }
                value = tempValue;
            }
            return String.Format(descriptionText, value);
        }
        
    }
}
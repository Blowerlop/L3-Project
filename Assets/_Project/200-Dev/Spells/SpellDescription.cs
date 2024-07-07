using System;
using System.Globalization;
using _Project._200_Dev.Entities.Player.Stats;
using UnityEngine;

namespace _Project._200_Dev.Spells
{
    [Serializable]
    public class SpellDescription
    {
        [TextArea(1, 5)] public string descriptionText;
        [SerializeReference] public Effect[] effects;
        public string GenerateText()
        {
            string result = string.Empty;
            object[] value = new object[effects.Length * 2];
            for (int i = 0; i < effects.Length ; i++)
            {
                value[i * 2] = effects[i].GetEffectValue().ToString(CultureInfo.InvariantCulture);
                value[i * 2 + 1] = effects[i].GetEffectDuration().ToString(CultureInfo.InvariantCulture);
            }
            if (effects.Length > value.Length)
            {
                object[] tempValue = new object[effects.Length];
                for (int i = 0; i < tempValue.Length; i++)
                {
                    if (i < value.Length)
                        tempValue[i] = (string)value[i];
                    else
                        tempValue[i] = "#MISSING_VALUE";
                }
                value = tempValue;
            }

            try
            {
                return string.Format(descriptionText, value);
            }
            catch (Exception e)
            {
                Debug.LogError("Error while formatting spell description: " + e.Message);
                return descriptionText;
            }
        }
        
    }
}

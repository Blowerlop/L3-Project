using System.Collections.Generic;
using System.Linq;
using System.Text;
using Project._Project.Scripts;
using Project.Extensions;
using Project.Spells;
using Sirenix.OdinInspector;
using UnityEditor;
using UnityEngine;

namespace Project
{
    public enum EAttackType
    {
        Melee,
        Ranged
    }
    
    [CreateAssetMenu(menuName = "Scriptable Objects/Character")]
    public class SOCharacter : SOEntity
    {
        [ShowInInspector, PropertyOrder(-1), ReadOnly] public int id = -1;

        [field: SerializeField, AssetsOnly, Required]
        [field: OnValueChanged("RenameAssetByName", InvokeOnInitialize = false, InvokeOnUndoRedo = true)]
        [field: OnValueChanged("SetId", InvokeOnInitialize = false,  InvokeOnUndoRedo = true)]
        public string characterName { get;  set; }
        [field: SerializeField, AssetsOnly, Required, PreviewField(75)] public Sprite avatar { get; private set; }
        [field: SerializeField, AssetsOnly, Required, PreviewField(75)] public Sprite avatar2 { get; private set; }
        [field: SerializeField, AssetsOnly, Required, PreviewField(75)] public GameObject model { get; private set; }
        [field: SerializeField, AssetsOnly, Required] public GameObject prefab { get; private set; }
        // [field: SerializeField, AssetsOnly, Required] public GameObject model { get; private set; }

        [RequiredListLength(SpellData.CharacterSpellsCount), SerializeField, InlineEditor]
        private SpellData[] spells = new SpellData[SpellData.CharacterSpellsCount];
        
        [field: SerializeReference, ListDrawerSettings(ShowIndexLabels = false, DraggableItems = false)] public StatBase[] stats;
        [field: SerializeField] public EAttackType attackType { get; private set; }
        [field: SerializeField, ShowIf("attackType", EAttackType.Ranged)] public SOProjectile projectileData;
        
        
        public static SOCharacter[] GetAllCharacters()
        {
            return SOScriptableObjectReferencesCache.GetScriptableObjects<SOCharacter>();
        }
        
        public static bool TryGetCharacter(int id, out SOCharacter characterData)
        {
            characterData = GetCharacter(id);
            return characterData != null;
        }

        public static SOCharacter GetCharacter(int id)
        {
            return GetAllCharacters().FirstOrDefault(character => character.id == id);
        }

        public static SOCharacter GetMobileCharacterData()
        {
            return GetAllCharacters().FirstOrDefault(character => character.characterName == "mobile_character");
        }
        
        public bool TryGetSpell(int index, out SpellData spell)
        {
            spell = null;
            if(index < 0 || index >= spells.Length) return false;

            spell = spells[index];
            return true;
        }

        public SpellData[] GetSpells()
        {
            return spells;
        }

        #region Editor
#if UNITY_EDITOR
        private void RenameAssetByName()
        {
            // string path = AssetDatabase.GetAssetPath(GetInstanceID());
            // AssetDatabase.RenameAsset(path, characterName);
        }
#endif
        
        private void SetId()
        {
            id = characterName.ToHashIsSameAlgoOnUnreal();
        }
        
        private void IsRanged() => attackType = EAttackType.Ranged;
        
#if UNITY_EDITOR
        [Button]
        private void FindIfCharactersHaveTheSameId()
        {
            Debug.Log("Start searching...");

            IEnumerable<SOCharacter> characters = GetAllCharacters();

            Dictionary<int, List<string>> ids = new Dictionary<int, List<string>>();
            foreach (SOCharacter character in characters)
            {
                if (ids.ContainsKey(character.id) == false)
                {
                    ids.Add(character.id, new List<string>());
                }
                
                ids[character.id].Add(character.name);
                
            }

            StringBuilder stringBuilder = new StringBuilder();
            int duplicatedIds = 0;
            
            foreach (var kvp in ids)
            {
                if (kvp.Value.Count == 1) continue;
                duplicatedIds++;
                stringBuilder.AppendLine(kvp.Key + ":");
                
                foreach (string value in kvp.Value)
                {
                    stringBuilder.AppendLine("- " + value);
                }

                stringBuilder.AppendLine();
            }
            
            Debug.Log($"Search ended ! {(duplicatedIds == 0 ? "No duplicated id." : $"{duplicatedIds} duplicated id:" + "\n" + stringBuilder.ToString())}");
        }
        
        [Button]
        [Tooltip("Sometimes Github don't detect the changes made on the ScriptableObject, so we need to force the write on the disk")]
        private void ForceSaveOnDisk()
        {
            EditorUtility.SetDirty(this);
            AssetDatabase.SaveAssets();
            AssetDatabase.Refresh();
        }
#endif
        #endregion
    }

    // I've not been able to override all ScriptableObject inspectors to display the button 
    
    // #if UNITY_EDITOR
    // [CustomEditor(typeof(ScriptableObject), true)]
    // public class ScriptableObjectEditor : OdinEditor
    // {
    //     public override void OnInspectorGUI()
    //     {
    //         base.OnInspectorGUI();
    //
    //         // SOCharacter t = target as SOCharacter;
    //         // if (t == null) return;
    //         Debug.Log("Working");
    //         
    //         if (GUILayout.Button("Force save on disk"))
    //         {
    //             EditorUtility.SetDirty(target);
    //             AssetDatabase.SaveAssets();
    //             AssetDatabase.Refresh();
    //         }
    //     }
    // }
    // #endif
}

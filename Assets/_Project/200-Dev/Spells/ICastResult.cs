using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Sirenix.OdinInspector;
using Unity.Netcode;

namespace _Project._200_Dev.Spells
{
    public interface ICastResult : INetworkSerializable
    {
        public static readonly IEnumerable<Type> AllResultTypes = AppDomain.CurrentDomain.GetAssemblies()
            .SelectMany(s => s.GetTypes())
            .Where(p => p != typeof(ICastResult) && typeof(ICastResult).IsAssignableFrom(p));
        
        public static IEnumerable AllResultTypesAsString = AllResultTypes
            .Select(p => new ValueDropdownItem() {Text = p.Name, Value = p.FullName});

        public static Type GetTypeFromName(string name)
        {
            return AllResultTypes.FirstOrDefault(x => x.Name == name);
        }
    }
}
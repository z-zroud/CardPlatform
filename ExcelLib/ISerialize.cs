using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace UtilLib
{
    public interface ISerialize
    {
        /// <summary>
        /// Serialize object to string
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        string Serialize(Object obj);

        /// <summary>
        /// Serialize object to file
        /// </summary>
        /// <param name="obj"></param>
        /// <param name="filePath"></param>
        void Serialize(Object obj, string filePath);

        /// <summary>
        /// Deserialize object from string
        /// </summary>
        /// <param name="xmlString"></param>
        /// <param name="type"></param>
        /// <returns></returns>
        object DeserializeFromString(string xmlString, Type type);

        /// <summary>
        /// Deserialize object from file
        /// </summary>
        /// <param name="filePath"></param>
        /// <param name="type"></param>
        /// <returns></returns>
        object DeserizlizeFromFile(string filePath, Type type);
    }
}

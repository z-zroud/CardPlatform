using System;
using System.IO;
using System.Text;
using System.Xml.Serialization;

namespace UtilLib
{
    public class XmlSerialize : ISerialize
    {
        private bool _bIncludeNamespace = false;

        public void SetNamespace(bool bIncludeNamespace = false)
        {
            _bIncludeNamespace = bIncludeNamespace;
        }

        /// <summary>
        /// Serialize object to string
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        public string Serialize(Object obj)
        {
            StreamWriter stWriter = null;
            XmlSerializer xmlSerializer;
            string buffer;
            try
            {
                xmlSerializer = new XmlSerializer(obj.GetType());
                MemoryStream memStream = new MemoryStream();
                stWriter = new StreamWriter(memStream);
                if (!_bIncludeNamespace)
                {
                    System.Xml.Serialization.XmlSerializerNamespaces xs = new XmlSerializerNamespaces();
                    xs.Add("", "");
                    xmlSerializer.Serialize(stWriter, obj, xs);
                }
                else
                {
                    xmlSerializer.Serialize(stWriter, obj);
                }
                buffer = Encoding.ASCII.GetString(memStream.GetBuffer());
            }
            catch (Exception Ex)
            {
                throw Ex;
            }
            finally
            {
                if (stWriter != null) stWriter.Close();
            }
            return buffer;
        }

        /// <summary>
        /// Serialize object to file
        /// </summary>
        /// <param name="obj"></param>
        /// <param name="filePath"></param>
        public void Serialize(Object obj, string filePath)
        {
            StreamWriter stWriter = null;
            XmlSerializer xmlSerializer;
            try
            {
                xmlSerializer = new XmlSerializer(obj.GetType());
                stWriter = new StreamWriter(filePath);
                if (!_bIncludeNamespace)
                {
                    System.Xml.Serialization.XmlSerializerNamespaces xs = new XmlSerializerNamespaces();
                    xs.Add("", "");//To remove namespace and any other inline information tag
                    xmlSerializer.Serialize(stWriter, obj, xs);
                }
                else
                {
                    xmlSerializer.Serialize(stWriter, obj);
                }
            }
            catch (Exception exception)
            {
                throw exception;
            }
            finally
            {
                if (stWriter != null) stWriter.Close();
            }
        }

        /// <summary>
        /// Deserialize object from string
        /// </summary>
        /// <param name="xmlString"></param>
        /// <param name="type"></param>
        /// <returns></returns>
        public object DeserializeFromString(string xmlString, Type type)
        {
            XmlSerializer xmlSerializer;
            MemoryStream memStream = null;
            try
            {
                xmlSerializer = new XmlSerializer(type);
                byte[] bytes = new byte[xmlString.Length];
                Encoding.ASCII.GetBytes(xmlString, 0, xmlString.Length, bytes, 0);
                memStream = new MemoryStream(bytes);
                object objectFromXml = xmlSerializer.Deserialize(memStream);
                return objectFromXml;
            }
            catch (Exception Ex)
            {
                throw Ex;
            }
            finally
            {
                if (memStream != null) memStream.Close();
            }
        }

        /// <summary>
        /// Deserilize object from file
        /// </summary>
        /// <param name="filePath"></param>
        /// <param name="type"></param>
        /// <returns></returns>
        public object DeserizlizeFromFile(string filePath, Type type)
        {
            XmlSerializer xmlSerializer;
            FileStream fileStream = null;
            try
            {
                xmlSerializer = new XmlSerializer(type);
                fileStream = new FileStream(filePath, FileMode.OpenOrCreate, FileAccess.ReadWrite);
                object objectFromXml = xmlSerializer.Deserialize(fileStream);
                return objectFromXml;
            }
            catch (Exception)
            {
                return null;
                //throw Ex;
            }
            finally
            {
                if (fileStream != null) fileStream.Close();
            }
        }
    }
}

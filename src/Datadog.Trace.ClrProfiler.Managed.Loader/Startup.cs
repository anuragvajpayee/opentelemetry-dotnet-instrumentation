using System;
using System.Reflection;

namespace Datadog.Trace.ClrProfiler.Managed.Loader
{
    /// <summary>
    /// A class that attempts to load the OpenTelemetry.AutoInstrumentation.ClrProfiler.Managed .NET assembly.
    /// </summary>
    public partial class Startup
    {
        /// <summary>
        /// Initializes static members of the <see cref="Startup"/> class.
        /// This method also attempts to load the OpenTelemetry.AutoInstrumentation.ClrProfiler.Managed .NET assembly.
        /// </summary>
        static Startup()
        {
            // Console.WriteLine("Startup Called - DD loader");
            ManagedProfilerDirectory = ResolveManagedProfilerDirectory();

            try
            {
                AppDomain.CurrentDomain.AssemblyResolve += AssemblyResolve_ManagedProfilerDependencies;
            }
            catch (Exception ex)
            {
                StartupLogger.Log(ex, "Unable to register a callback to the CurrentDomain.AssemblyResolve event.");
            }

            TryLoadManagedAssembly();
        }

        internal static string ManagedProfilerDirectory { get; }

        private static void TryLoadManagedAssembly()
        {
            try
            {
                // var assembly = Assembly.Load("OpenTelemetry.AutoInstrumentation.ClrProfiler.Managed, Version=1.24.0.0, Culture=neutral, PublicKeyToken=34b8972644a12429");
                var assembly = Assembly.Load("Inception.ClrProfiler.Managed, Version=1.0.0.0, Culture=neutral, PublicKeyToken=d1cede69117c04c0");

                if (assembly != null)
                {
                    // call method Datadog.Trace.ClrProfiler.Instrumentation.Initialize()
                    // var type = assembly.GetType("Datadog.Trace.ClrProfiler.Instrumentation", throwOnError: false);
                    var type = assembly.GetType("Inception.ClrProfiler.Instrumentation", throwOnError: false);
                    var method = type?.GetRuntimeMethod("Initialize", parameters: new Type[0]);
                    method?.Invoke(obj: null, parameters: null);
                }
            }
            catch (Exception ex)
            {
                StartupLogger.Log(ex, "Error when loading managed assemblies.");
            }
        }

        private static string ReadEnvironmentVariable(string key)
        {
            try
            {
                return Environment.GetEnvironmentVariable(key);
            }
            catch (Exception ex)
            {
                StartupLogger.Log(ex, "Error while loading environment variable " + key);
            }

            return null;
        }
    }
}

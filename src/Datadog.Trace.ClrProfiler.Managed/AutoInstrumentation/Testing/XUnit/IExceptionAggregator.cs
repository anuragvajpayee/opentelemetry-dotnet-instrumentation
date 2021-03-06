using System;

namespace Datadog.Trace.ClrProfiler.AutoInstrumentation.Testing.XUnit
{
    /// <summary>
    /// Exception aggregator interface
    /// </summary>
    public interface IExceptionAggregator
    {
        /// <summary>
        /// Extract exception
        /// </summary>
        /// <returns>Exception instance</returns>
        Exception ToException();
    }
}

/**********************************************************************************
 * MIT License
 *
 * Copyright (c) 2018 Antoine Beauchamp
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

#ifndef SA_APP_H
#define SA_APP_H

#include "ILogger.h"
#include "IRegistryService.h"

#include <string>

namespace shellanything
{
  /// <summary>
  /// Manages the system's current ILogger instance.
  /// </summary>
  class SHELLANYTHING_EXPORT App
  {
  public:
    static App& GetInstance();
  private:
    App();
    ~App();

    // Disable copy constructor and copy operator
    App(const App&);
    App& operator=(const App&);

  public:

    /// <summary>
    /// Gets the path of the current application or module running ShellAnything. Must be set manually by the application.
    /// </summary>
    /// <returns>Returns the value of the path. Returns an empty string if never set.</returns>
    const std::string& GetApplicationPath();

    /// <summary>
    /// Sets the path of the current application or module running ShellAnything. Must be set manually by the application.
    /// </summary>
    /// <param name="value">The value of the path.</param>
    void SetApplicationPath(const std::string& value);

    /// <summary>
    /// Set the current application logger.
    /// </summary>
    /// <remarks>
    /// If a logger instance is already set, the caller must properly destroy the old instance.
    /// </remarks>
    /// <param name="logger">A valid instance of a ILogger.</param>
    void SetLogger(ILogger* logger);

    /// <summary>
    /// Get the current application logger
    /// </summary>
    /// <returns>Returns a pointer to an ILogger instance. Returns NULL if no logger is set.</returns>
    ILogger* GetLogger();

    /// <summary>
    /// Set the current application registry service.
    /// </summary>
    /// <remarks>
    /// If a service instance is already set, the caller must properly destroy the old instance.
    /// </remarks>
    /// <param name="instance">A valid instance of a the service.</param>
    void SetRegistry(IRegistryService* instance);

    /// <summary>
    /// Get the current application registry service.
    /// </summary>
    /// <returns>Returns a pointer of the instance that is currently set. Returns NULL if no service is set.</returns>
    IRegistryService* GetRegistry();

    /// <summary>
    /// Test if application is loaded in a test environment (main's tests executable).
    /// </summary>
    /// <returns>Returns true if application is loaded in a test environment. Returns false otherwise.</returns>
    bool IsTestingEnvironment();

    /// <summary>
    /// Get the application's log directory. The returned directory has write access.
    /// </summary>
    /// <returns>Returns the path of the directory that should be used by the logging framework.</returns>
    std::string GetLogDirectory();

    /// <summary>
    /// Get the application's configurations directory.
    /// </summary>
    /// <returns>Returns the path of the directory of the user's Configuration Files.</returns>
    std::string GetConfigurationsDirectory();

    /// <summary>
    /// Test if the given directory is valid for logging.
    /// </summary>
    /// <returns>Returns true if the directory is valid for logging. Returns false otherwise.</returns>
    bool IsValidLogDirectory(const std::string& path);

    /// <summary>
    /// Initialize and start the application ready for usage.
    /// </summary>
    /// <returns>Returns true if init and start has succeeded. Returns false otherwise.</returns>
    bool Start();

  private:
    /// <summary>
    /// Install the original configuration files to the specified destination directory.
    /// </summary>
    /// <param name="dest_dir">The destination directory.</param>
    void InstallDefaultConfigurations(const std::string& dest_dir);

    /// <summary>
    /// Initialize the Configuration Manager to the user's stall the original configuration files to the specified destination directory.
    /// </summary>
    void InitConfigManager();

    /// <summary>
    /// Setup default properties for the application.
    /// </summary>
    void SetupGlobalProperties();

  private:
    std::string mApplicationPath;

    ILogger * mLogger;
    IRegistryService* mRegistry;
  };


} //namespace shellanything

#endif //SA_APP_H

/*
  In App.xaml:
  <Application.Resources>
      <vm:ViewModelLocator xmlns:vm="clr-namespace:CardPlatform"
                           x:Key="Locator" />
  </Application.Resources>
  
  In the View:
  DataContext="{Binding Source={StaticResource Locator}, Path=ViewModelName}"

  You can also use Blend to do all this with the tool's support.
  See http://www.galasoft.ch/mvvm
*/

using CommonServiceLocator;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Ioc;
//using Microsoft.Practices.ServiceLocation;

namespace CardPlatform.ViewModel
{
    /// <summary>
    /// This class contains static references to all the view models in the
    /// application and provides an entry point for the bindings.
    /// </summary>
    public class ViewModelLocator
    {
        /// <summary>
        /// Initializes a new instance of the ViewModelLocator class.
        /// </summary>
        public ViewModelLocator()
        {
            ServiceLocator.SetLocatorProvider(() => SimpleIoc.Default);

            ////if (ViewModelBase.IsInDesignModeStatic)
            ////{
            ////    // Create design time view services and models
            ////    SimpleIoc.Default.Register<IDataService, DesignDataService>();
            ////}
            ////else
            ////{
            ////    // Create run time view services and models
            ////    SimpleIoc.Default.Register<IDataService, DataService>();
            ////}

            SimpleIoc.Default.Register<MainViewModel>();
            SimpleIoc.Default.Register<PersonlizeViewModel>();
            SimpleIoc.Default.Register<AlgorithomViewModel>();
            SimpleIoc.Default.Register<CardCheckViewModel>();
            SimpleIoc.Default.Register<TransactionViewModel>();
            SimpleIoc.Default.Register<TerminalViewModel>();
        }

        public MainViewModel Main
        {
            get
            {
                return ServiceLocator.Current.GetInstance<MainViewModel>();
            }
        }

        public TerminalViewModel Terminal
        {
            get
            {
                return ServiceLocator.Current.GetInstance<TerminalViewModel>();
            }
        }

        public PersonlizeViewModel Personlize
        {
            get
            {
                return ServiceLocator.Current.GetInstance<PersonlizeViewModel>();
            }
        }

        public AlgorithomViewModel Algorithm
        {
            get
            {
                return ServiceLocator.Current.GetInstance<AlgorithomViewModel>();
            }
        }

        public CardCheckViewModel CardCheck
        {
            get
            {
                return ServiceLocator.Current.GetInstance<CardCheckViewModel>();
            }
        }

        public TransactionViewModel Transaction
        {
            get
            {
                return ServiceLocator.Current.GetInstance<TransactionViewModel>();
            }
        }

        public static void Cleanup()
        {
            // TODO Clear the ViewModels
        }
    }
}
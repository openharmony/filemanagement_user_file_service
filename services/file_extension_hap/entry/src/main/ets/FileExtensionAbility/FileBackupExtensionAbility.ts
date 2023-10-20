import type { BundleVersion } from '@ohos.application.BackupExtensionAbility';
import BackupExtensionAbility from '@ohos.application.BackupExtensionAbility';


const TAG = 'FileBackupExtAbility';

export default class FileBackupExtAbility extends BackupExtensionAbility {
  async onBackup() : Promise<void> {
    console.log(TAG, 'onBackup ok.');
  }

  async onRestore(bundleVersion : BundleVersion) : Promise<void> {
    console.log(TAG, `onRestore ok ${JSON.stringify(bundleVersion)}`);
  }
}

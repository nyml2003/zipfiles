import { FileDetail } from "@/apis/GetFileDetail";
import { GetFileListRequest } from "@/apis/GetFileList";
import { FileType } from "@/types";

export function selectEnvironment<T>(production: T, development: T) {
  if (process.env.BASE_ENV === 'prod') {
    return production;
  } else {
    return development;
  }
}

export function isProduction() {
  return process.env.BASE_ENV === 'prod';
}

export function isDevelopment() {
  return process.env.BASE_ENV === 'dev';
}

export function cleanObject(obj: any) {
  return Object.entries(obj).reduce((acc: any, [key, value]) => {
    if (value && typeof value === 'object') {
      value = cleanObject(value);
    }
    if (value !== null && value !== undefined) {
      acc[key] = value;
    }
    return acc;
  }, {});
}

export const filterBy = <T extends FileDetail>(files: T[], filter: Partial<GetFileListRequest['filter']>): T[] => {
  const cleanedFilter = cleanObject(filter);
  if (!cleanedFilter || Object.keys(cleanedFilter).length === 0) return files;

  return files.filter(file => {
    if (file.type === FileType.Directory) return true;
    if (cleanedFilter.type && file.type !== cleanedFilter.type) return false;
    if (cleanedFilter.size) {
      if (cleanedFilter.size.min && file.size < cleanedFilter.size.min) return false;
      if (cleanedFilter.size.max && file.size > cleanedFilter.size.max) return false;
    }
    if (cleanedFilter.owner && !file.owner.includes(cleanedFilter.owner)) return false;
    if (cleanedFilter.group && !file.group.includes(cleanedFilter.group)) return false;
    if (cleanedFilter.owner) {
      console.log(file.owner, cleanedFilter.owner);
    }
    return true;
  });
};